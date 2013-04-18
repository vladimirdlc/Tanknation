//ClanLib
#include <ClanLib/core.h>
#include <ClanLib/sound.h>
#include <ClanLib/display.h>
#include <ClanLib/database.h>
#include <ClanLib/sqlite.h>
#include <ClanLib/gui.h>

//C++
#include <cstdlib> 
#include <ctime>
#include <list>

#include "mundo.h"
#include "Tanque.h"
#include "construccion.h"
#include "misil.h"

//MySQL
#include <mysql++.h>

#define ID_JUGADOR_LOCAL 0
#define TIEMPOCHAT 7000
#define FRAMERATE 80

using namespace std;

Mundo::Mundo(CL_DisplayWindow &display_window) : ventana(display_window), quit(false)
{
	ventana.flip(1);
	gc = ventana.get_gc();
	errormysql = false;
	menuingame = false;
	menuinicial = true;
	menumulti = false;
	noupdatedb = false;
	deathmatchIA = false;
    act_segundos = false;
	cambiartextbox = false;
	muestra_est = true;
	DM_Multi = false;
	contSubID = 0;
	
	recursos = new CL_ResourceManager("recursos.xml");

	sonidocomienzo = new CL_SoundBuffer("Comienzo", recursos);
	sonidocomienzo->set_volume(0.3f);
	sonidocomienzo->prepare();

	musica = new CL_SoundBuffer("Musica", recursos);
	sonidocomienzo->set_volume(0.5f);
	sonidocomienzo->prepare();

	sonido_ataereo = new CL_SoundBuffer("Ataque_Aereo", recursos);
	sonido_ataereo->set_volume(1.0f);
	sonido_ataereo->prepare();

	gdte = CL_Gradient(CL_Colorf(0.0f,0.0f,0.0f,0.8f), CL_Colorf(0.0f,0.0f,0.0f,0.9f));



	//Fondo
	fondo = CL_Texture("Fondo", recursos, gc);

	window_rect = ventana.get_viewport();

	teclado = ventana.get_ic().get_keyboard();
	mouse = ventana.get_ic().get_mouse();
	ventana.hide_cursor();
	
	//Slot Salir
	slot_quit = ventana.sig_window_close().connect(this, &Mundo::on_window_close);

	// Slots Teclado
	slotKeyDown = teclado.sig_key_down().connect(this, &Mundo::onKeyDown);
	slotKeyUp = teclado.sig_key_up().connect(this, &Mundo::onKeyUp);

	//Slots Mouse
	slotMouseDown = mouse.sig_key_down().connect(this, &Mundo::onMouseDown);
	slotMouseMove = mouse.sig_pointer_move().connect(this, &Mundo::onMouseMove);
	
	//GUI
	Cursor = CL_Sprite(gc, "Cursor", recursos);
	LogoMini = CL_Sprite(gc, "LogoMini", recursos);
	Boton = CL_Sprite(gc, "Boton", recursos);
	Boton_ok = CL_Sprite(gc, "Boton_ok", recursos);
	Boton_cancelar = CL_Sprite(gc, "Boton_cancelar", recursos);
	Boton_cambiar = CL_Sprite(gc, "Boton_cambiar", recursos);
	AyudaCambio = CL_Sprite(gc, "AyudaCambio", recursos);
	AyudaComo = CL_Sprite(gc, "AyudaComo", recursos);
	AyudaDia = CL_Sprite(gc, "AyudaDia", recursos);
	AyudaNada = CL_Sprite(gc, "AyudaNada", recursos);
	ind_menu = 0;
	
	ftextura22 = CL_Font_System(gc, L"Arial", 22);
	ftextura20 = CL_Font_System(gc, L"Arial", 20);
	ftextura10 = CL_Font_System(gc, L"Arial", 10);

	cadenachat = "";
	cadenatiempo="";
	NombreJugador = "TN_Player"; //10 Caracteres maximo
	IP_Conectar = "tanknation.servegame.org";
	IP_Conectar2 = IP_Conectar;
	puedescribir = false;
	borrarchat = false;
	tcadenachat  = 0;


	EsServidor = true;
	EsCliente = !EsServidor;
	NumJugadores = 7;

	TanqueJugador  = new Tanque(DEFAULT,false, this);


	if(EsServidor)
	{
		servidor =  new Servidor(this,NumJugadores);
		TipoConex = "Servidor";
	}
	else
	{
		cliente = new Cliente(this);
		TipoConex = "Cliente";
	}
	
	frame_handler.set_fps_limit(FRAMERATE);

	iniciarNivel();

}

void Mundo::iniciarNivel()
{
	SpawnPoints = vector<CL_Vec2f>();
	SpawnPoints.push_back(CL_Vec2f(50,50));
	SpawnPoints.push_back(CL_Vec2f(60,600));
	SpawnPoints.push_back(CL_Vec2f(150,500));
	SpawnPoints.push_back(CL_Vec2f(900,500));
	SpawnPoints.push_back(CL_Vec2f(300,300));
	SpawnPoints.push_back(CL_Vec2f(300,700));
	SpawnPoints.push_back(CL_Vec2f(60,200));
	SpawnPoints.push_back(CL_Vec2f(200,20));
	SpawnPoints.push_back(CL_Vec2f(400,600));
	SpawnPoints.push_back(CL_Vec2f(600,200));
	SpawnPoints.push_back(CL_Vec2f(600,80));
	SpawnPoints.push_back(CL_Vec2f(800,100));
	SpawnPoints.push_back(CL_Vec2f(900,20));
	SpawnPoints.push_back(CL_Vec2f(900,700));


	if(EsServidor)	
	{
		agregarTanque(ID_JUGADOR_LOCAL,DEFAULT,NombreJugador,true,false); //Agrega el tanque nuevo
	}

	setReloj(0,0,false);	//Tiempo de ronda
	temp = CL_System::get_time();
	
	sonidocomienzo->play();
	musica->play(true);

	tiempopowerup = 0;

}

bool Mundo::revisarPosicion(CL_Vec2f _pos, int id) //revisa posición de tanques
{
	
	std::list<Tanque *>::iterator it;
	for(it = tanques.begin(); it != tanques.end(); it++)
	{			
		if((*it)->getCuerpoCollision().point_inside(CL_Pointf(_pos)) && (*it)->getID() != id)
		{
			//(*it)->getID()!=idtanque)&&
			return false; //si está ocupda
		}
	}

	return true;
}

CL_Vec2f Mundo::getPosicionSpawn() 
{
	vector<CL_Vec2f> SpawnPointsLibres;
	srand((unsigned)time(0));

	std::list<Tanque *>::iterator it;
	vector<CL_Vec2f >::iterator it2;

	bool temp=false;

		for(it2 = SpawnPoints.begin(); it2 != SpawnPoints.end(); it2++)
		{	
			for(it = tanques.begin(); it != tanques.end(); it++)
			{			
					if((*it)->getCuerpoCollision().point_inside(CL_Pointf((*it2))))
					{
						temp =true;
						break;
					}
			}

			if(!temp)
				SpawnPointsLibres.push_back((*it2));

			temp = false;
		}



	int indice = rand()%(SpawnPointsLibres.size() - 1);


	return SpawnPointsLibres[indice]; 
}

void Mundo::agregarObjeto(GameObject *object)
{
	objetos.push_back(object);
}

CL_Vec2f Mundo::agregarTanque(int id, TanqueTipo tipo, CL_String nombre, bool esjugador, bool _esIA) 
{
	Tanque *tank  = new Tanque(tipo,_esIA, this);

	tank->setPos(getPosicionSpawn());
	
	tank->setID(id);
	tank->setNombre(nombre);
	tank->setSalud(60,0,100,100);

	if(!_esIA && esjugador) //Si es el jugador local
	{
		tank->seleccionar();
		TanqueJugador = tank;
	}
	objetos.push_back(tank);
	tanques.push_back(tank);

	return tank->getPos();
}

void Mundo::agregarTanque(int id, TanqueTipo tipo, CL_String nombre, bool esjugador, 
						  bool _esIA, CL_Vec2f _pos,float _angulo,float _angulotorreta) 
{
	Tanque *tank  = new Tanque(tipo,_esIA, this);

	tank->setPos(_pos);
	tank->setAngulo(_angulo);
	tank->setAnguloTorreta(_angulotorreta);

	tank->setID(id);
	tank->setNombre(nombre);
	tank->setSalud(60,0,100,100);

	if(!_esIA && esjugador) //Si es el jugador local
	{
		tank->seleccionar();
		TanqueJugador = tank;
	}

		objetos.push_back(tank);
		tanques.push_back(tank);
	
}

void Mundo::quitarTanque(int id)
{
	std::list<Tanque *>::iterator it;
	for(it = tanques.begin(); it != tanques.end(); ++it)
		if((*it)->getID()==id)
		{
			break;
		}

	std::list<GameObject *>::iterator it2;
	for(it2 = objetos.begin(); it2 != objetos.end(); ++it2)
	{
		if( (*it2)->getID()==id )
		{
			delete (*it2);
			it = tanques.erase(it);
			it2 = objetos.erase(it2);
			break;
		}
	}	
}

void Mundo::quitarTodosExcJugador()
{	
	powerups.erase(powerups.begin(), powerups.end());
	tanques.erase(tanques.begin(), tanques.end());
	objetos.erase(objetos.begin(), objetos.end());
	agregarTanque(ID_JUGADOR_LOCAL,DEFAULT,NombreJugador,true,false); //Agrega el tanque nuevo}	
}


void Mundo::agregarPowerup(PowerupTipo tipo,CL_Vec2f _pos)
{	
	Powerup *poder  = new Powerup(tipo, this);
	poder->setPos(_pos);
	poder->setID(tipo + powerups.size());
	objetos.push_back(poder);
	powerups.push_back(poder);
}

void Mundo::otorgaPowerup(int id_tanque, int id_power)
{
	quitarPowerup(id_power);

	if(puedeActualizar())
	{
		servidor->otorga_powerup(id_tanque, id_power);
	}
	else
	{
		if (id_power >= 40 && id_power < 50) //velocidad_tanque
		{
			getTanqueID(id_tanque)->hiperVelocidad();
		}
		else if (id_power >= 50 && id_power < 60) //disparo_360
		{
			getTanqueID(id_tanque)->disparo360();
		}
	}

}

void Mundo::quitarPowerup(int id)
{
	std::list<Powerup *>::iterator it;
	for(it = powerups.begin(); it != powerups.end(); ++it)
		if( (*it)->getID()==id )
		{
			break;
		}
		
	std::list<GameObject *>::iterator it2;
	for(it2 = objetos.begin(); it2 != objetos.end(); ++it2)
	{
		if( (*it2)->getID()==id )
		{
			delete (*it2);
			it = powerups.erase(it);
			it2 = objetos.erase(it2);
			break;
		}
	}
}

void Mundo::quitarTodosPowerup()
{
	if(puedeActualizar())
	{
		servidor->quitaTodosPowerup();
	}

	std::list<Powerup *>::iterator it;
	std::list<GameObject *>::iterator it2;
	for(it = powerups.begin(); it != powerups.end(); ++it)
	{
		for(it2 = objetos.begin(); it2 != objetos.end(); ++it2)
		{
			if( (*it2)->getID()==(*it)->getID() )
			{
				delete (*it2);
				it2 = objetos.erase(it2);
				break;
			}
		}
	}

	powerups.erase(powerups.begin(), powerups.end());

}

void Mundo::quitarObjeto(int id)
{
	std::list<GameObject *>::iterator it2;
	for(it2 = objetos.begin(); it2 != objetos.end(); ++it2)
	{
		if( (*it2)->getID()==id )
		{
			delete (*it2);
			it2 = objetos.erase(it2);
			break;
		}
	}
	
}

GameObject* Mundo::getObjeto(int id)
{
	std::list<GameObject *>::iterator it2;
	for(it2 = objetos.begin(); it2 != objetos.end(); ++it2)
	{
		if( (*it2)->getID()==id )
		{
			return (*it2);
		}
	}

	return NULL;
}


Tanque* Mundo::getTanqueID(int idtanque)
{
	std::list<Tanque *>::iterator it;
	for(it = tanques.begin(); it != tanques.end(); ++it)
	{
		if((*it)->getID()==idtanque)
		{
			return (*it);
		}
	}

	return NULL;
}

CL_Vec2f Mundo::getPowerupPosMin(PowerupTipo tipo,Tanque *tanque)
{
	 CL_Vec2f posmin = CL_Vec2f(0.0f,0.0f);
	 float distmin = 2000.0f;

	std::list<Powerup *>::iterator it;
	for(it = powerups.begin(); it != powerups.end(); ++it)
	{
		if((*it)->getTipo()==tipo)
		{
			float distemp= tanque->getPos().distance((*it)->getPos());
			if(distemp < distmin)
			{
				distmin = distemp;
				posmin = (*it)->getPos();
			}
			
		}
	}

	return posmin;
}

CL_Vec2f Mundo::getPosMinTanque(Tanque *tanque)
{
	 CL_Vec2f posmin = CL_Vec2f(0.0f,0.0f);
	 float distmin = 2000.0f;
	 int idtanque = tanque->getID();

	std::list<Tanque *>::iterator it;
	for(it = tanques.begin(); it != tanques.end(); ++it)
	{
		if(((*it)->getID()!=idtanque)&&(!(*it)->estaMuerto()))
		{
			float distemp=tanque->getPos().distance((*it)->getPos());
			if(distemp < distmin)
			{
				distmin = distemp;
				posmin = (*it)->getPos();
			}
			
		}
	}

	return posmin;
	
}

void Mundo::chequeaReloj()
{
	if(rlj_minutos < 0) //Evento Reinicio
	{
		//setReloj(0,0,false);	//Tiempo de ronda

		rlj_segundos = 0;
		rlj_minutos = 0;
		act_segundos = false;

		sonido_ataereo->play();


		if(!DM_Multi)
		{
			std::list<CL_Pointf>::iterator it;

			for(it = puntos_explosion.begin(); it != puntos_explosion.end(); ++it)
			{
				efectoExplosion((*it));
			}
		}


		if(puedeActualizar())
		{
			if(TanqueJugador->getScore() > 0 && !noupdatedb) //Si hizo record y se puede actualizar bdd
			{
				CL_String dir_recservidor = "tanknation.servegame.org", tipojuego = "Deathmatch IA";
				if(DM_Multi) tipojuego = "DeathMatch Multi";

				char *cString;
				
				if(IP_Conectar == "localhost")
					cString = "127.0.0.1";
				else
					cString = "tanknation.servegame.org";
				//Upload record
		/*			if(!errormysql)
				{
				try
					{

						mysqlpp::Connection con(false);
						if (con.connect("tanknation_record", cString, "root", "my19504841")) 
						{
							mysqlpp::Query query = con.query();
							//query.exec("insert into score (apodo, record, tipo) values ('"+TanqueJugador->getNombre()+"',"+CL_String((TanqueJugador->getScore())+", 'Deathmatch IA')"));
							if(query.exec(cl_format("INSERT INTO score (apodo, record, tipo, fechahora) VALUES ('%1',%2,'%3', NOW())", TanqueJugador->getNombre(), TanqueJugador->getScore(), tipojuego))) 
							{ //Si ejecutó el query
								AgregarCadenaChat("Sistema: Record subido al servidor Global");
								AgregarCadenaChat("<"+dir_recservidor+">");
							}
							else
							{
								AgregarCadenaChat("Sistema: No se pudo conectar al servidor Global");
								AgregarCadenaChat("<"+dir_recservidor+">");
								errormysql = true;
							}
	 
							if(tipojuego == "Deathmatch IA")
								query << "SELECT * FROM score WHERE tipo = 'Deathmatch IA' ORDER BY record DESC LIMIT 3";  
							else if(tipojuego == "DeathMatch Multi")
								query << "SELECT * FROM score WHERE tipo = 'DeathMatch Multi' ORDER BY record DESC LIMIT 3";  

							mysqlpp::StoreQueryResult res = query.store();

							CL_String topes = "Top Jugadores Global:";
							AgregarCadenaChat(topes);
							topes = "";

							if (res) {
								//Muestra el resultado
								for (unsigned int i = 0; i < res.num_rows(); i++) {
									
									topes += cl_format("(%1)%2 %3 ", (1+i), CL_String(res[i][0]), CL_String(res[i][1])); //apodo
								}
							}
							else {
								// Retreive failed
								topes += "No se obtuvo ninguno";
							}

							AgregarCadenaChat(topes);
						}
						else //No se pudo conectar al servidor global
						{
							AgregarCadenaChat("Sistema: No se pudo conectar al servidor Global");
							AgregarCadenaChat("<"+dir_recservidor+">");
							errormysql = true;
						}
					}
					catch(...)
					{
							AgregarCadenaChat("Sistema: No se pudo conectar al servidor Global ");
							AgregarCadenaChat("<"+dir_recservidor+">");
							errormysql = true;
					}
				}*/

				CL_String bdlocal = "tanknationlocal.bd";
				CL_SqliteConnection connection(bdlocal);
				
				CL_DBCommand command = connection.create_command("INSERT INTO score (apodo, record, tipo, fechahora) VALUES (?1,?2,?3, DATETIME('NOW'))");
				command.set_input_parameter_string(1, TanqueJugador->getNombre());
				command.set_input_parameter_int(2, TanqueJugador->getScore());
				command.set_input_parameter_string(3, tipojuego);
				connection.execute_non_query(command);

				command = connection.create_command("SELECT * FROM score WHERE tipo = '"+tipojuego+"' ORDER BY record DESC LIMIT 3");
				CL_DBReader reader = connection.execute_reader(command);

				CL_String topes = "Top Jugadores Local: ";
				AgregarCadenaChat(topes);
				topes = "";

				int i = 1;
				while (reader.retrieve_row())
				{
					//Muestra el resultado
						topes += cl_format("(%1)%2 %3, ",  i, reader.get_column_string(0), reader.get_column_int(1)); //apodo
						i++;
				}
				reader.close();

				AgregarCadenaChat(topes);
			}

			//bye bye bots
			if(deathmatchIA)
			{
				quitarTanque(1);
				quitarTanque(2);
				quitarTanque(3);
			}

			reseteaTanques();
		}
		else //Es cliente
		{
			if(TanqueJugador->getScore() > 0 && !noupdatedb) //Si hizo record y se puede actualizar bdd
			{
				CL_String dir_recservidor = "tanknation.servegame.org", tipojuego = "Deathmatch IA";
				if(!DM_Multi) 
				//Upload record
				if(!errormysql)
				{
					mysqlpp::Connection con(false);
					if (con.connect("tanknation_record", "tanknation.servegame.org", "root", "my19504841")) 
					{
						mysqlpp::Query query = con.query();
 
						if(tipojuego == "Deathmatch IA")
							query << "SELECT * FROM score WHERE tipo = 'Deathmatch IA' ORDER BY record DESC LIMIT 3";  
						else if(tipojuego == "DeathMatch Multi")
							query << "SELECT * FROM score WHERE tipo = 'DeathMatch Multi' ORDER BY record DESC LIMIT 3";  

						mysqlpp::StoreQueryResult res = query.store();

						CL_String topes = "Top Jugadores Global:";
						AgregarCadenaChat(topes);
						topes = "";

						if (res) {
							//Muestra el resultado
							for (unsigned int i = 0; i < res.num_rows(); i++) {
								
								topes += cl_format("(%1)%2 %3 ", (1+i), CL_String(res[i][0]), CL_String(res[i][1])); //apodo
							}
						}
						else {
							// Retreive failed
							topes += "No se obtuvo ninguno";
						}

						AgregarCadenaChat(topes);
					}
					else //No se pudo conectar al servidor global
					{
						AgregarCadenaChat("Sistema: No se pudo conectar al servidor Global");
						AgregarCadenaChat("<"+dir_recservidor+">");
						errormysql = true;
					}
				}

				CL_String bdlocal = "tanknationlocal.bd";
				CL_SqliteConnection connection(bdlocal);
				CL_DBCommand command = connection.create_command("SELECT * FROM score WHERE tipo = '"+tipojuego+"' ORDER BY record DESC LIMIT 3");
				CL_DBReader reader = connection.execute_reader(command);

				CL_String topes = "Top Jugadores Local: ";
				AgregarCadenaChat(topes);
				topes = "";

				int i = 1;
				while (reader.retrieve_row())
				{
					//Muestra el resultado
						topes += cl_format("(%1)%2 %3, ",  i, reader.get_column_string(0), reader.get_column_int(1)); //apodo
						i++;
				}
				reader.close();

				AgregarCadenaChat(topes);
			}
		}
		rlj_minutos = 3;
		quitarTodosPowerup();
	}
}

bool Mundo::estoySolo(Tanque *tanque)
{
	 int idtanque = tanque->getID();

	std::list<Tanque *>::iterator it;
	for(it = tanques.begin(); it != tanques.end(); ++it)
	{
		if(((*it)->getID()!=idtanque)&&(!(*it)->estaMuerto()))
		{
			return false; //no está sólo
		}
	}

	return true; //está sólo
}

Tanque* Mundo::getTanqueJugador()
{
	return TanqueJugador;
}

void Mundo::reseteaTanques()
{
	std::list<Tanque *>::iterator it;
	for(it = tanques.begin(); it != tanques.end(); ++it)
	{
		 (*it)->setScore(0);
		 (*it)->setParametros(TanqueJugador->getTipo());
		 (*it)->Matar(CL_System::get_time(),5000);
		 matarTanque((*it)->getID(),5000);	
	}
}

Tanque* Mundo::getTanquePosMin(Tanque *tanque)
{
	 Tanque *tanquetemp = new Tanque(DEFAULT,false,this);
	 float distmin = 2000.0f;
	 int idtanque = tanque->getID();

	std::list<Tanque *>::iterator it;
	for(it = tanques.begin(); it != tanques.end(); ++it)
	{
		if(((*it)->getID()!=idtanque)&&(!(*it)->estaMuerto()))
		{
			float distemp=tanque->getPos().distance((*it)->getPos());
			if(distemp < distmin)
			{
				distmin = distemp;
				tanquetemp = (*it);
			}
			
		}
	}

	if(distmin == 2000.0f)
	{
		tanquetemp->setPos(getPosicionSpawn());
	}


	return tanquetemp;		 

}

CL_Vec2i Mundo::getReloj()
{
	return CL_Vec2i(rlj_minutos,rlj_segundos);
}

void Mundo::matarTanque(int id, int tiempofin)
{
	if(puedeActualizar())		
		servidor->matar_tanque(id, tiempofin);
}

void Mundo::matarMisil(int id)
{
	if(puedeActualizar())		
		servidor->matar_misil(id);
}

void Mundo::actualiza_saludyscore()
{
	if(puedeActualizar())		
		servidor->actualiza_saludyscore();
}


void Mundo::actualiza_posiciones()
{
	if(puedeActualizar())		
		servidor->actualiza_posiciones();
}

void Mundo::setReloj(int _minutos, int _segundos)
{
	rlj_minutos = _minutos;
	if(_segundos > 60000)
	{
		rlj_segundos = 60000;
	}
	else
	{
		rlj_segundos = _segundos;
	}

	rlj_asc = true;
}

void Mundo::setReloj(int _minutos, int _segundos, bool es_ascendente)
{
	rlj_minutos = _minutos;
	if(_segundos > 60000)
	{
		rlj_segundos = 60000;
	}
	else
	{
		rlj_segundos = _segundos;
	}

	rlj_asc = es_ascendente;

	if(rlj_segundos == 60000 && !rlj_asc) 
	{
		rlj_minutos--;
	}

	
}


int Mundo::revisarColision(CL_CollisionOutline *outline, GameObject *otro)
{

	std::list<GameObject *>::iterator it;
	for(it = objetos.begin(); it != objetos.end(); ++it)
	{
		if( (*it) != otro )
		{
			if((*it)->revisarColision(outline, otro))
			{
				return (*it)->getID();
			}
				
		}
	}
	

	return -1;
}

CL_String Mundo::getNombreJugador()
{
	return NombreJugador;
}

bool Mundo::puedeActualizar()
{
	if((EsServidor && DM_Multi) || deathmatchIA)
		return true;
	else
		return false;
}


void Mundo::onKeyDown(const CL_InputEvent &key, const CL_InputState &state)
{
	CL_Vec2f posmouse((float)key.mouse_pos.x,(float)key.mouse_pos.y); 
	TanqueJugador->setTargetPosTorreta(posmouse);
	  
	//Menu en juego
	if(key.id == CL_KEY_ESCAPE)
	{
		if(!menuinicial && !menumulti)
			menuingame = !menuingame;
		
		if(menuinicial) //Salir
		{
			quit = true; 
		}

		if(menumulti)
		{
			menuinicial = true;
			menumulti = false;
		}
	}

	if((key.id==CL_KEY_ENTER) && puedescribir)
	{

		if(EsServidor)
		{
			//enviar = true;	
			   //verifica los tipos de comandos
				bool nuevo_nombre = false;

				if((cadenachat.length() > 9) && cadenachat.substr(0,8) == "nnombre ")
				{
					NombreJugador = cadenachat.substr(8,cadenachat.length());
					TanqueJugador->setNombre(cadenachat.substr(8,cadenachat.length()));
					nuevo_nombre = true;
				}

					AgregarCadenaChat(TanqueJugador->getNombre() + ":" + cadenachat);
					servidor->enviaratodos(CL_NetGameEvent("Juego-Msj",TanqueJugador->getNombre() + ":" + cadenachat));
					cadenachat.clear();
					puedescribir = false;
		}
		else
		{
			   //verifica los tipos de comandos
				if((cadenachat.length() > 9) && cadenachat.substr(0,8) == "nnombre ")
				{
					NombreJugador = cadenachat.substr(8,cadenachat.length());
					TanqueJugador->setNombre(cadenachat.substr(8,cadenachat.length()));
				}
				if((cadenachat.length() > 5) && cadenachat.substr(0,4) == "nip ")
				{
					//cliente->ip_servidor = cadenachat.substr(4,cadenachat.length());
					cliente->conectar_a_servidor();
				}


					AgregarCadenaChat(TanqueJugador->getNombre() + ":" + cadenachat);
					cliente->cliente_red.send_event(CL_NetGameEvent("Juego-Msj",TanqueJugador->getNombre() + ":" + cadenachat));
					cadenachat.clear();
					puedescribir = false;			
		}
	}
	
	if(puedescribir)
	{
		if(key.id==CL_KEY_SPACE)
		{
			cadenachat +=" ";
		}	
		else if(key.id==CL_KEY_BACKSPACE) 
		{
			if(!cadenachat.empty())
			{
				cadenachat.erase(cadenachat.size()-1);
			}
		}
		else //Muestra todos los demás carácteres
		{
			cadenachat.append(key.str);
		}
	}


	else
	{
		if(key.id == CL_KEY_W/*P1_ADELANTE*/|| key.id == CL_KEY_UP)
			{		
				
				TanqueJugador->movAdelante();

				if(EsCliente)
					cliente->cliente_red.send_event(CL_NetGameEvent("Juego-ActualizarTeclado",0,key.id));
				else
					servidor->enviaratodos(CL_NetGameEvent("Juego-ActualizarTeclado",1,0,key.id));

				/*if(key.id == CL_KEY_UP)
				{
					key = new CL_InputEvent();
				}*/
				
			}
		if(key.id== CL_KEY_S/*P1_ATRAS*/|| key.id == CL_KEY_DOWN)
			{
				TanqueJugador->movAtras();
				
				if(EsCliente)
					cliente->cliente_red.send_event(CL_NetGameEvent("Juego-ActualizarTeclado",0,key.id));
				else
					servidor->enviaratodos(CL_NetGameEvent("Juego-ActualizarTeclado",1,0,key.id));

			}
			if(key.id == CL_KEY_D/*P1_DERECHA*/|| key.id == CL_KEY_RIGHT)
			{
				TanqueJugador->movDer();

				if(EsCliente)
					cliente->cliente_red.send_event(CL_NetGameEvent("Juego-ActualizarTeclado",0,key.id));
				else
					servidor->enviaratodos(CL_NetGameEvent("Juego-ActualizarTeclado",1,0,key.id));

			}
			if(key.id == CL_KEY_A/*P1_IZQUIERDA*/|| key.id == CL_KEY_LEFT)
			{	
				TanqueJugador->movIzq();

				if(EsCliente)
					cliente->cliente_red.send_event(CL_NetGameEvent("Juego-ActualizarTeclado",0,key.id));
				else
					servidor->enviaratodos(CL_NetGameEvent("Juego-ActualizarTeclado",1,0,key.id));
				
			}
				
			if(key.id == CL_KEY_SPACE)
			{	
				if(puedeActualizar())
					TanqueJugador->disparar();
	
				if(EsCliente)
					cliente->cliente_red.send_event(CL_NetGameEvent("Juego-ActualizarTeclado",0,key.id));
				else
				{
					servidor->enviaratodos(CL_NetGameEvent("Juego-ActualizarTeclado",1,0,key.id));
				}
			}

	}

	if(key.id==CL_KEY_TAB) 
	{
		muestra_est = !muestra_est;
	}
	if(key.id==CL_KEY_Y) 
	{
		puedescribir=true;
	}

	if(menuinicial)
	{
		if(cambiartextbox) //Si se desea cambiar el nickname
		{
			if(key.id==CL_KEY_SPACE)
			{
				//No haga nada
			}	
			if(key.id==CL_KEY_ENTER)
			{
				cambiartextbox = false;
				NombreJugador = TanqueJugador->getNombre();
			}	
			else if(key.id==CL_KEY_BACKSPACE) 
			{ 
				TanqueJugador->nombre.erase(TanqueJugador->nombre.size()-1);
			}
			else //Muestra todos los demás carácteres
			{
				TanqueJugador->nombre.append(key.str);
			}		
		}
	}

	if(menumulti)
	{
		if(cambiartextbox) //Si se desea cambiar el nickname
		{
			if(key.id==CL_KEY_SPACE)
			{
				//No haga nada
			}	
			if(key.id==CL_KEY_ENTER)
			{
				cambiartextbox = false;
				IP_Conectar = IP_Conectar2;
			}	
			else if(key.id==CL_KEY_BACKSPACE) 
			{ 
				IP_Conectar2.erase(IP_Conectar2.size()-1);
			}
			else //Muestra todos los demás carácteres
			{
				IP_Conectar2.append(key.str);
			}		
		}
	}

	/*if(key.id==CL_KEY_R && !puedescribir) 
	{
		if(deathmatchIA) //Sólo en dIA
		{
			rlj_minutos = -1;
			chequeaReloj();
		}
	}*/

	/*if(key.id==CL_KEY_P)
	{
		bt = true;
		t = CL_System::get_time();
	}*/

	}
	

void Mundo::teclatanquedown(int idtanque,int tecla) 
{
	std::list<Tanque *>::iterator it;
	for(it = tanques.begin(); it != tanques.end(); ++it)
		if((*it)->getID()==idtanque)
		{
			if(tecla == CL_KEY_W/*P1_ADELANTE*/|| tecla == CL_KEY_UP)
			{		
				(*it)->movAdelante();
			}
			if(tecla == CL_KEY_S/*P1_ATRAS*/|| tecla == CL_KEY_DOWN)
			{
				(*it)->movAtras();

			}
			if(tecla == CL_KEY_D/*P1_DERECHA*/|| tecla == CL_KEY_RIGHT)
			{
				(*it)->movDer();

			}
			if(tecla == CL_KEY_A/*P1_IZQUIERDA*/|| tecla == CL_KEY_LEFT)
			{	
				(*it)->movIzq();
			}
			if(tecla == CL_KEY_SPACE)
			{
				if(puedeActualizar())
					(*it)->disparar();
			}

			break;
		}
}

void Mundo::onKeyUp(const CL_InputEvent &key, const CL_InputState &state)
{

			if(key.id== CL_KEY_W/*P1_ADELANTE*/|| key.id == CL_KEY_UP)
			{
				TanqueJugador->noMovAdelante();

				if(EsCliente)
					cliente->cliente_red.send_event(CL_NetGameEvent("Juego-ActualizarTeclado",1,key.id));
				else
					servidor->enviaratodos(CL_NetGameEvent("Juego-ActualizarTeclado",1,1,key.id));

			}
			if(key.id == CL_KEY_S/*P1_ATRAS*/|| key.id == CL_KEY_DOWN)
			{
				TanqueJugador->noMovAtras();

				if(EsCliente)
						cliente->cliente_red.send_event(CL_NetGameEvent("Juego-ActualizarTeclado",1,key.id));
				else
						servidor->enviaratodos(CL_NetGameEvent("Juego-ActualizarTeclado",1,1,key.id));

			}
			if(key.id == CL_KEY_D/*P1_DERECHA*/|| key.id == CL_KEY_RIGHT)
			{
				TanqueJugador->noMovDer();

				if(EsCliente)
						cliente->cliente_red.send_event(CL_NetGameEvent("Juego-ActualizarTeclado",1,key.id));
				else
						servidor->enviaratodos(CL_NetGameEvent("Juego-ActualizarTeclado",1,1,key.id));


			}
			if(key.id == CL_KEY_A/*P1_IZQUIERDA*/|| key.id == CL_KEY_LEFT)
			{
				TanqueJugador->noMovIzq();

				if(EsCliente)
						cliente->cliente_red.send_event(CL_NetGameEvent("Juego-ActualizarTeclado",1,key.id));
				else
						servidor->enviaratodos(CL_NetGameEvent("Juego-ActualizarTeclado",1,1,key.id));

			}

}


void Mundo::teclatanqueup(int idtanque,int tecla) 
{
	std::list<Tanque *>::iterator it;
		for(it = tanques.begin(); it != tanques.end(); ++it)
			if((*it)->getID()==idtanque)
			{
				if(tecla== CL_KEY_W/*P1_ADELANTE*/ || tecla == CL_KEY_UP)
				{
					(*it)->noMovAdelante();
				}
				if(tecla == CL_KEY_S/*P1_ATRAS*/ || tecla == CL_KEY_DOWN)
				{
					(*it)->noMovAtras();
				}
				if(tecla == CL_KEY_D/*P1_DERECHA*/ || tecla == CL_KEY_RIGHT)
				{
					(*it)->noMovDer();

				}
				if(tecla== CL_KEY_A/*P1_IZQUIERDA*/ || tecla == CL_KEY_LEFT)
				{
					(*it)->noMovIzq();
				}

				break;
			}
}


void Mundo::onMouseDown(const CL_InputEvent &key, const CL_InputState &state)
{
	if(key.id == CL_MOUSE_LEFT)
	{
			if(puedeActualizar() || menuinicial || menumulti)
				TanqueJugador->disparar();

			if(EsCliente)
				cliente->cliente_red.send_event(CL_NetGameEvent("Juego-ActualizarMouse",TanqueJugador->getID()));
			else
				servidor->enviaratodos(CL_NetGameEvent("Juego-ActualizarMouse",TanqueJugador->getID()));

			bool chkmenu = false;

			if(ind_menu != 0 && !chkmenu) //Si se presionó el mouse en el menu ayuda
			{
				if(CL_Rect(41, 675, 41+214, 675+330).contains(mouse.get_position())) //Boton salir
					ind_menu = 2; //Como
				else if(CL_Rect(41+214+20, 675, 41+428+20, 675+330).contains(mouse.get_position())) //Boton salir
					ind_menu = 3; //Tipos
				else if(CL_Rect(41+428+20, 675, 41+428+214+20, 675+330).contains(mouse.get_position())) //Boton salir
					ind_menu = 4; //cambiar
				else if(CL_Rect(41+428+214, 675, 41+428*2+20, 675+330).contains(mouse.get_position())) //Boton salir
					ind_menu = 0; //regresar
			    
				chkmenu = true;
			}

			if(menuingame && !chkmenu) //Si se presionó el mouse en el menu ingame
			{
				if(CL_Rect(428, 229, 229+428, 73+229).contains(mouse.get_position())) //Boton regresar
					menuingame = false;
				if(CL_Rect(428, 229+110, 229+428, 73+229+110).contains(mouse.get_position())) //Nueva Partida
				{
					deathmatchIA = false;
					DM_Multi = false;
					menuinicial = true;
					menuingame = false;
					noupdatedb = true;
					rlj_minutos = -1;
					chequeaReloj();
					noupdatedb = false;
					quitarTodosExcJugador();
				}
				if(CL_Rect(428, 229+220, 229+428, 73+229+220).contains(mouse.get_position())) //Ayuda
				{
					ind_menu = 1;
				}
				if(CL_Rect(428, 229+330, 229+428, 73+229+330).contains(mouse.get_position())) //Boton salir
					quit = true;

				chkmenu = true;
			}

			if(menuinicial && !chkmenu) //Si se presionó el mouse en el menu inicial
			{
				if(CL_Rect(428, 229, 229+428, 73+229).contains(mouse.get_position())) //Si se indico DeathMatchIA
				{
					if(!EsServidor) //Si ya es servidor no cambie nada
					{
						EsServidor = true;
						EsCliente = false;
						if(cliente != NULL) //Si no se creo un cliente
							delete cliente;
						servidor =  new Servidor(this,NumJugadores);
						TipoConex = "Servidor";
					}
					noupdatedb = true;
					rlj_minutos = -1;
					chequeaReloj();
					noupdatedb = false;
					deathmatchIA = true;
					DM_Multi = false;
					menuinicial = false;
					
				}
					
				if(CL_Rect(428, 229+110, 229+428, 73+229+110).contains(mouse.get_position())) //Multiplayer
				{
					menuinicial = false;
					menumulti = true;
				}

				if(CL_Rect(428, 229+220, 229+428, 73+229+220).contains(mouse.get_position())) //Ayuda
				{
					ind_menu = 1;
				}


				if(CL_Rect(259+15, 700+15, 287+15, 730+15).contains(mouse.get_position())) //Boton cambiar
				{
					cambiartextbox = !cambiartextbox;
					if(!cambiartextbox) //Si se aceptó 
						NombreJugador = TanqueJugador->getNombre();
				}
				
				if(CL_Rect(259+50, 700+15, 287+50, 730+15).contains(mouse.get_position())) //Boton Cancelar
				{
					cambiartextbox = false;
					TanqueJugador->setNombre(NombreJugador);
				}

				if(CL_Rect(428, 229+330, 229+428, 73+229+330).contains(mouse.get_position())) //Boton salir
					quit = true;

				chkmenu = true;
			}

			if(menumulti && !chkmenu) //Si se presionó mouse en menu multijugador
			{
				if(CL_Rect(428, 229, 229+428, 73+229).contains(mouse.get_position())) //Si se indico DeathMatchIA
				{
					noupdatedb = true;
					rlj_minutos = -1;
					chequeaReloj();
					noupdatedb = false;
					menumulti = false;
					DM_Multi = true;
					deathmatchIA = false;
					if(!EsServidor) //Si ya es servidor no cambie nada
					{
						EsServidor = true;
						EsCliente = false;
						if(cliente != NULL) //Si no se creo un cliente
							delete cliente;
						servidor =  new Servidor(this,NumJugadores);
						TipoConex = "Servidor";
					}
				}

				if(CL_Rect(428, 229+110, 229+428, 73+229+110).contains(mouse.get_position())) //Multiplayer Cte
				{
					noupdatedb = true;
					rlj_minutos = -1;
					chequeaReloj();
					noupdatedb = false;
					menumulti = false;
					DM_Multi = true;
					if(!EsCliente) //Si ya es cliente no cambie nada
					{
						EsServidor = false;
						EsCliente = true;
						if(servidor != NULL)
							delete servidor;
						cliente = new Cliente(this);					
						TipoConex = "Cliente";
						cliente->ip_servidor = IP_Conectar;
						cliente->conectar_a_servidor();
					}
				}

				if(CL_Rect(428, 229+220, 229+428, 73+229+220).contains(mouse.get_position())) //Ayuda
				{
					ind_menu = 1;
				}

				if(CL_Rect(428, 229+330, 229+428, 73+229+330).contains(mouse.get_position())) //Regresa
				{
					menuinicial = true;
					menumulti = false;
				}

				if(CL_Rect(259+15, 700+15, 287+15, 730+15).contains(mouse.get_position())) //Boton cambiar
				{
					cambiartextbox = !cambiartextbox;
					if(!cambiartextbox) //Si se aceptó 
						IP_Conectar = IP_Conectar2;
				}
				
				if(CL_Rect(259+50, 700+15, 287+50, 730+15).contains(mouse.get_position())) //Boton Cancelar
				{
					cambiartextbox = false;
					IP_Conectar2 = IP_Conectar;
				}

				chkmenu = true;
			}

			
	}

}

void Mundo::onMouseMove(const CL_InputEvent &key, const CL_InputState &state)
{
	CL_Vec2f posmouse((float)key.mouse_pos.x,(float)key.mouse_pos.y); 
	TanqueJugador->setTargetPosTorreta(posmouse);

	/*if(EsCliente)
		cliente->cliente_red.send_event(CL_NetGameEvent("Juego-ActualizarMouse",posmouse.x,posmouse.y));
	else
		servidor->enviaratodos(CL_NetGameEvent("Juego-ActualizarMouse",1,posmouse.x,posmouse.y));
		*/


}

void Mundo::gameloop()
{
	
         // Inicializa el motor de partículas
         srand(CL_System::get_time());
         L_ParticleSystem::init();

         // Crear una superficie para se usada por la particula y establecer el alineamiento
         CL_Sprite particula_1_sur(gc, "Ruido", recursos);
         particula_1_sur.set_alignment(origin_center);

         CL_Sprite particula_2_sur(gc, "Explosion", recursos);
         particula_2_sur.set_alignment(origin_center);

         /* Si se usa L_INFINITE_LIFE el sistema no removerá
         la partícula automáticamente */
         particula_1 = new L_Particle(&particula_1_sur, L_INFINITE_LIFE);
         particula_1->set_color(L_Color(255,255,150,70));
		 

         // Set callback function.
//         particula_1->set_prerun_callback(&efectoExplosion, (void*)this);


         // Setup shooting effect.
         L_Vector shooting_vector;
         shooting_vector.set2(0.4f, (float)L_DEGREE_TO_RADIAN(-90) );
         disparo_efecto = new L_ShootingEffect(320,400,shooting_vector,40,1);
         disparo_efecto->add(particula_1);
         disparo_efecto->set_angle_interval((float)L_DEGREE_TO_RADIAN(25));
         disparo_efecto->set_follow_shooting();
         disparo_efecto->initialize();


         // Setup explosion effect
         motion_ctrl.set_1d_acceleration((float)-0.0004);
         particula_2 = new L_Particle(&particula_2_sur,500);
         particula_2->set_color( L_Color(255,110,60,255) );
         particula_2->coloring2( L_Color(255,255,255,100), L_Color(0,255,60,60) );
         //particula_2->sizing2( 0.8, 1.5 );
		 particula_2->sizing2(1.0, 2.5);
         particula_2->set_motion_controller(&motion_ctrl);

         exp_efecto = new L_ExplosionEffect(0,0,16,4,5,0.15f);
         exp_efecto->add(particula_2);
         exp_efecto->set_life(100); //set life of this effect
         exp_efecto->set_rotation_distortion(L_2PI);
         exp_efecto->set_size_distortion(0.7f);
         exp_efecto->set_life_distortion(100); //set life distortion for particles
         exp_efecto->set_speed_distortion(0.1f);
         exp_efecto->initialize();

         exp_emisor  = new L_EffectEmitter(exp_efecto);

		 for(int i = 0; i <= 1024; i+=64)
			for(int j = 0; j <= 768; j+=64)
			{
				puntos_explosion.push_back(CL_Pointf((float)i,(float)j));
			}

	//GameLoop
	while(!quit)
	{	
		gc.clear();

		CL_KeepAlive::process();

/*		if(EsServidor)
			servidor->procesareventos();
		else
			cliente->procesareventos();
			*/

		update();
/*
		if(EsServidor)
			servidor->procesareventos();
		else
			cliente->procesareventos();*/
		CL_KeepAlive::process();

		draw();
		

		//if(EsServidor)
		//	servidor->procesareventos();
		//else
		//	cliente->procesareventos();

		//if(EsServidor)
		//	servidor->procesareventos();
		//else
		//	cliente->procesareventos();

		//CL_KeepAlive::process();
		//CL_KeepAlive::process();

	}

	matarParticulas();

	
}

void Mundo::update()
{
	float dt = calcularTiempoTranscurrido();


	//Tiempo del programa

	segundos = CL_System::get_time() - temp;

	frame_handler.limit_frame();
    frame_handler.calculate();

	if(puedeActualizar()) //Si es servidor
	{               
			tiempopowerup +=dt*1000;

			if(tiempopowerup>=10000)
			{
				srand((unsigned)time(0));

				tiempopowerup =0;

				PowerupTipo tipos[4] = {MEDIKIT,ESCUDO,RAPIDEZ_TANQUE,DISPARO_360};

				int i = rand()%4;

				CL_Vec2f pos = getPosicionSpawn();
				agregarPowerup(tipos[i],pos);

				if(puedeActualizar())
					servidor->agrega_powerup(i, pos.x, pos.y);
			}

		if(rlj_asc) //Para un reloj ascendente
		{
			rlj_segundos = segundos;
			if(segundos >=60000)
			{
				temp=CL_System::get_time();
				segundos = 0;
				rlj_minutos++;
			}
		}
		else //Reloj descendente
		{
			if(act_segundos)
				rlj_segundos = 60000 - segundos;

			if(rlj_segundos <= 0)
			{
				temp=CL_System::get_time();
				segundos = 0;
				rlj_segundos = 60000;
				rlj_minutos--;
				act_segundos = true;
			}
		}
		
		servidor->actualiza_tiempo();
		chequeaReloj();

	}
	
	if(borrarchat&&(CL_System::get_time()-tcadenachat >= TIEMPOCHAT)) 
	{
		if(!VChat.empty())
		{
			int asd = VChat.size();

			if(asd > 1)
			{
				int tamañochat = VChat.size() -1;
				for(int i= 0;  i < tamañochat; i++) 
				{
					VChat[i] = VChat[i+1];
				}
				VChat.erase(VChat.end() -1);
			}
			else
				VChat.erase(VChat.begin());

			tcadenachat = CL_System::get_time();
		}
		else
		{
			borrarchat=false;
		}
	}

	std::list<GameObject *>::iterator it;
	for(it = objetos.begin(); it != objetos.end(); )
	{

		if((*it)->update(dt) == false)
		{
			delete (*it);
			it = objetos.erase(it);
		}
		else
			++it;
	}

	std::list<Powerup *>::iterator it2;
	for(it2 = powerups.begin(); it2 != powerups.end(); ++it2)
	{
		(*it2)->update(dt);
	}

	//Se actualizan las posiciones para evitar error
		if(DM_Multi)
			if(EsServidor)
				servidor->actualiza_posiciones();
			else
				cliente->actualiza_posiciones();

   
	//************************ DEMO D.IA ************************/
	//Agrega los tanques enemigos de acuerdo al nivel	
	if(deathmatchIA) //Si se activó el modo de juego D.IA
	{
		int recjugador = TanqueJugador->getScore(); 
		if(recjugador < 7 && recjugador == tanques.size() -1)
		{
			switch(recjugador) //Bots
			{
			case 0: 
				agregarTanque(1,BRU,"Bru",false,true,getPosicionSpawn(),90,90); break;
			case 1:
				agregarTanque(2,VOLDO,"Voldo",false,true,getPosicionSpawn(),90,90); break;
			case 2:
				agregarTanque(3,ROSA,"Rosa",false,true,getPosicionSpawn(),90,90); break;
			}
		}
	}
	 //++++++++++++++++++++++FIN D.IA+++++++++++++++++++++//
}

// Calcula cuanto ha pasado desde el último frame
float Mundo::calcularTiempoTranscurrido()
{
	static float lastTime = 0;

	float newTime = (float)CL_System::get_time();
	if(lastTime == 0)
		lastTime = newTime;

	float deltaTime = (newTime - lastTime) / 1000.0f;
	lastTime = newTime;

	return deltaTime;
}

void Mundo::draw()
{

		//Dibuja el fondo
		gc.set_texture(0, fondo);
		CL_Draw::texture(gc, window_rect);	
		gc.reset_texture(0);
		
		//Dibuja todos los objetos del juego
		std::list<GameObject *>::iterator it;
			for(it = objetos.begin(); it != objetos.end(); ++it)
				(*it)->draw();
	
		
		//GUI

	    //ftextura22.draw_text(gc,10,30, cl_format("%1 FPS",frame_handler.get_fps()),CL_Colorf::greenyellow);
     	
		//ftextura.draw_text(gc,10,50,cl_format("%1",TipoConex),CL_Colorf::greenyellow);
				
		if(rlj_minutos < 10)
			cadenatiempo = "0%1:";
		else
			cadenatiempo = "%1:";

		if(rlj_segundos < 10000)
			cadenatiempo +="0%2";
		else
			cadenatiempo +="%2";


		if(rlj_minutos < 1)
			ftextura22.draw_text(gc,30,45, cl_format(cadenatiempo,rlj_minutos,rlj_segundos/1000),CL_Colorf::red);
		else
			ftextura22.draw_text(gc,30,45, cl_format(cadenatiempo,rlj_minutos,rlj_segundos/1000),CL_Colorf::greenyellow);
	
		//Mensajes Chat
		if(puedescribir)
			ftextura20.draw_text(gc, 10, 720,"Decir:" + cadenachat, CL_Colorf::whitesmoke);

		if(!VChat.empty())
		{
			int j=1;
			int tamañochat = VChat.size();
			for(int i = 0; i < tamañochat; i++)
			{
				ftextura20.draw_text(gc,10,600 + 20*j,VChat[i],CL_Colorf::whitesmoke);
				j++; //2
			}
		}

		int i = 0;

		std::list<Tanque *>::iterator it2;

		for(it2 = tanques.begin(); it2 != tanques.end(); ++it2)
		{	
			/*ftextura.draw_text(gc,10,300 + 20*i, cl_format("Tanque(%1): Posicion: %2,%3 AnguloCuerpo: %4 AnguloTorrt: %5"
															,(*it2)->getID(),(*it2)->getPos().x,(*it2)->getPos().y
															,(*it2)->getanguloCuerpo(),(*it2)->getTurretAngle()),CL_Colorf::greenyellow);*/		

			ftextura10.draw_text(gc,(int)((*it2)->getPos().x)-10,((int)(*it2)->getPos().y)-30, cl_format("%1",(*it2)->getNombre()),CL_Colorf::whitesmoke);
			ftextura22.draw_text(gc,(int)((*it2)->getPos().x)-6,((int)(*it2)->getPos().y)-8, cl_format("%1",CreaBarrita((*it2)->getVida())),CL_Colorf::greenyellow);
			ftextura22.draw_text(gc,(int)((*it2)->getPos().x)-6,((int)(*it2)->getPos().y)-12, cl_format("%1",CreaBarrita((*it2)->getArmadura())),CL_Colorf::aquamarine);
			if(muestra_est)
			{
				ftextura20.draw_text(gc,820,50 + 20*i, cl_format("%1", (*it2)->getNombre()),CL_Colorf::greenyellow);
				ftextura20.draw_text(gc,920,50 + 20*i, cl_format("Score: %1", (*it2)->getScore()),CL_Colorf::greenyellow);
				i++;
			}
		}

			//ftextura.draw_text(gc,720,30 + 20*i, cl_format("Vida:%1 Armadura:%2 Score:%3",(*it2)->getVida()
			//														,(*it2)->getArmadura(),(*it2)->getScore()),CL_Colorf::greenyellow);
			//ftextura = CL_Font_System(gc, L"Arial", 22);

	//Motor de Partículas
		exp_emisor->run(frame_handler.get_elapsed_time());
		L_DrawParticle(gc,exp_emisor);



	if(menuingame) //Si se activo el menú en el juego
	{		
		CL_Draw::gradient_fill(gc, window_rect, gdte);	
		LogoMini.draw(gc, 510, 120);
		for(int i = 0; i < 4; i++)
			Boton.draw(gc, 525, 250+(i*110));	

		ftextura22.draw_text(gc,485,251, "Continuar", CL_Colorf::white);
		ftextura22.draw_text(gc,469,251+110, "Nueva Partida", CL_Colorf::white);
		ftextura22.draw_text(gc,495,251+220, "Ayuda", CL_Colorf::white);
		ftextura22.draw_text(gc,463,251+330, "Salir del Juego", CL_Colorf::white);
	}

	if(menuinicial) //Si se activo el menú inicial
	{		
		CL_Draw::gradient_fill(gc, window_rect, gdte);	
		LogoMini.draw(gc, 510, 120);
		for(int i = 0; i < 4; i++)
			Boton.draw(gc, 525, 250+(i*110));	

		ftextura22.draw_text(gc,464,251, "DeathMatch IA", CL_Colorf::white);
		ftextura22.draw_text(gc,473,251+110, "Multijugador", CL_Colorf::white);
		ftextura22.draw_text(gc,496,251+220, "Ayuda", CL_Colorf::white);
		ftextura22.draw_text(gc,467,251+330, "Salir del Juego", CL_Colorf::white);
		ftextura20.draw_text(gc,52,685, "Nombre de Jugador", CL_Colorf::white);
		//CL_Draw::box(gc, 259, 700, 287, 730, CL_Colorf::aquamarine);
		if(!cambiartextbox)
		{
			ftextura20.draw_text(gc,54,720, TanqueJugador->getNombre(), CL_Colorf::goldenrod);
			Boton_cambiar.draw(gc,276,715);
		}
		else
		{
			ftextura20.draw_text(gc,54,720,  TanqueJugador->getNombre(), CL_Colorf::goldenrod);
			Boton_ok.draw(gc,276,715);
			Boton_cancelar.draw(gc,276+40,715);

			if((segundos % 3) == 0 || (segundos % 4) == 0)
			{
				ftextura20.draw_text(gc,54,720, TanqueJugador->getNombre()+"|", CL_Colorf::goldenrod);
			}
		}
		CL_Draw::box(gc, 50, 700, 50+200, 700+30, CL_Colorf::whitesmoke);	
	}

	if(menumulti)
	{
		CL_Draw::gradient_fill(gc, window_rect, gdte);	
		LogoMini.draw(gc, 510, 120);
		for(int i = 0; i < 4; i++)
			Boton.draw(gc, 525, 250+(i*110));	

		ftextura22.draw_text(gc,469,251, "Ser Servidor", CL_Colorf::white);
		ftextura22.draw_text(gc,465,251+110, "Conectarse a...", CL_Colorf::white);
		ftextura22.draw_text(gc,495,251+220, "Ayuda", CL_Colorf::white);
		ftextura22.draw_text(gc,485,251+330, "Regresar", CL_Colorf::white);
		
		ftextura20.draw_text(gc,52,685, "IP de Coneccion", CL_Colorf::white);
		//CL_Draw::box(gc, 259, 700, 287, 730, CL_Colorf::aquamarine);
		if(!cambiartextbox)
		{
			ftextura20.draw_text(gc,54,720, IP_Conectar2, CL_Colorf::goldenrod);
			Boton_cambiar.draw(gc,276,715);
		}
		else
		{
			ftextura20.draw_text(gc,54,720,  IP_Conectar2, CL_Colorf::goldenrod);
			Boton_ok.draw(gc,276,715);
			Boton_cancelar.draw(gc,276+40,715);

			if((segundos % 3) == 0 || (segundos % 4) == 0)
			{
				ftextura20.draw_text(gc,54,720, IP_Conectar2+"|", CL_Colorf::goldenrod);
			}
		}

		CL_Draw::box(gc, 50, 700, 50+200, 700+30, CL_Colorf::whitesmoke);	

	}

	switch(ind_menu)
	{
	case 1:	AyudaNada.draw(gc, 512, 383);
		break;
	case 2: AyudaComo.draw(gc, 512, 383);
		break;
	case 3: AyudaDia.draw(gc, 512, 383);
		break;
	case 4: AyudaCambio.draw(gc, 512, 383);
		break;
	}

	//ftextura22.draw_text(gc,100,70,cl_format("Cursor: X=%1, Y=%2",mouse.get_x(),mouse.get_y()),CL_Colorf::greenyellow);
	Cursor.draw(gc, (float)mouse.get_x() - Cursor.get_width()/2,(float)mouse.get_y() - Cursor.get_height()/2);

	ventana.flip(0);

}

void Mundo::AgregarCadenaChat(CL_String cadena)
{
	if(VChat.size() == 5)
		{
			VChat.erase(VChat.begin());
		}

		VChat.push_back(cadena);
		tcadenachat = CL_System::get_time();
		borrarchat = true;
}


CL_String Mundo::CreaBarrita(int tam) //Crea una barra de salud
{
	if(tam > 0)
	{
		CL_String barra = ""; 
		tam /= 10; //carita /=
		for(int i = 0; i < tam; i++)
		{
			barra += "'";
		}

		return barra;
	}	
	else return "";
}

void Mundo::on_window_close()
{
	quit = true;	
}

int Mundo::getSubID()
{
	contSubID++;
	return contSubID;
}

int Mundo::getActualSubID()
{
	return contSubID;
}

void Mundo::actualiza_SubID(int id_tanque)
{
	if(puedeActualizar())
		servidor->actualiza_SubID(id_tanque);
}

void Mundo::setSubID(int contador)
{
	contSubID = contador;
}


void Mundo::efectoExplosion(CL_Pointf pto)
{
//	exp_emisor:: emitter->emit(key.mouse_pos.x, key.mouse_pos.y);
	exp_emisor->emit(pto.x, pto.y);
}


void Mundo::matarParticulas() //Desinicializa particulas
{
         L_ParticleSystem::deinit();

         delete exp_emisor;
         delete exp_efecto;
         delete particula_2;
         delete disparo_efecto;
         delete particula_1;
}

Mundo::~Mundo()
{	
	// Borra todos los objetos
	std::list<GameObject *>::iterator it;
	for(it = objetos.begin(); it != objetos.end(); ++it)
		delete (*it);
}

