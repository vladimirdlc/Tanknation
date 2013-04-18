#include "servidor.h"
#include "usuario_servidor.h"

#include "mundo.h"
#include "Tanque.h"

class Mundo;
class Tanque;

Servidor::Servidor(Mundo* mun,int _numjugadores)
: siguiente_usuario_id(1)
{
	slots.connect(servidor_red.sig_client_connected(), this, &Servidor::on_cliente_conectado);
	slots.connect(servidor_red.sig_client_disconnected(), this, &Servidor::on_cliente_desconectado);
	slots.connect(servidor_red.sig_event_received(), this, &Servidor::on_evento_recibido);

	eventos_login.func_event("Login").set(this, &Servidor::on_evento_login);
	eventos_juego.func_event("Juego-Comenzar").set(this, &Servidor::on_evento_juego_solicitudcomenzar);
	eventos_juego.func_event("Juego-Msj").set(this,&Servidor::on_evento_juego_mensajechat);
	eventos_juego.func_event("Juego-ActualizarTeclado").set(this,&Servidor::on_evento_juego_actualizar_teclado);
	eventos_juego.func_event("Juego-ActualizarMouse").set(this,&Servidor::on_evento_juego_actualizar_mouse);
	eventos_juego.func_event("Juego-ActualizarPosiciones").set(this,&Servidor::on_evento_juego_actualizar_posiciones);

	mundo = mun;
	numjugadores = _numjugadores;


	cl_log_event("Sistema", "Servidor Inicializado");

	puerto = "4556";
	servidor_red.start(puerto);

	
}
Servidor::Servidor() 
{	

}

Servidor::~Servidor()
{
		
}


void Servidor::parar()
{
	servidor_red.stop();
}

void Servidor::procesareventos()
{
	servidor_red.process_events();
}


//Cliente Conectado
void Servidor::on_cliente_conectado(CL_NetGameConnection *conexion)
{
	cl_log_event("Red", "Cliente Conectado");

	//Crea el usuario y lo une a la conección
	UsuarioServidor *usuario = new UsuarioServidor();
	usuario->adjuntar_conexion(conexion);
}

//Cliente Desconectado
void Servidor::on_cliente_desconectado(CL_NetGameConnection *conexion)
{
	cl_log_event("Red", "Cliente Desconectado");
	
	UsuarioServidor *usuario = UsuarioServidor::get_usuario(conexion);

	CL_String mensaje = "Se desconecto " + usuario->nombre_usuario;
	mundo->AgregarCadenaChat(mensaje);
	enviaratodos(CL_NetGameEvent("Juego-Msj",mensaje));
	enviaratodos(CL_NetGameEvent("Juego-JugadorDesconectado",usuario->id));

	mundo->quitarTanque(usuario->id);
	
	Usuarios.remove(usuario);
}


void Servidor::quitaTodosPowerup()
{
	cl_log_event("eventos", "Quita Todos Powerup");
	enviaratodos(CL_NetGameEvent("Juego-QuitaTodosPowerup")); //actuales [todos excepto a mi]
}

//Evento Cliente: Recibido
void Servidor::on_evento_recibido(CL_NetGameConnection *conexion, const CL_NetGameEvent &e)
{

	UsuarioServidor *usuario = UsuarioServidor::get_usuario(conexion);

	if(usuario)
	{
		bool evento_manejado = false;

		if (usuario->id == 0)	// Usuario no se ha logeado, enrutar a eventos del despachador de login
		{
			//Despachador de eventos de login
			evento_manejado = eventos_login.dispatch(e, usuario);
		}
		else
		{
			//Despachador de eventos del juego
			evento_manejado = eventos_juego.dispatch(e, usuario);
		}

		if (!evento_manejado)
		{
			//Evento no manejado 
			cl_log_event("eventos", "Evento no manejado: %1", e.to_string());
		}
	}
}

//Evento Login: Recibido
void Servidor::on_evento_login(const CL_NetGameEvent &e, UsuarioServidor *usuario)
{
	cl_log_event("eventos", "Cliente solicita login");

	int numusuarios = Usuarios.size();
/*
	if(numusuarios < (numjugadores -1) )
	{
		CL_String idlogin = e.get_argument(0);
		CL_String password = e.get_argument(1);
		CL_String nombre_usuario = e.get_argument(2);

		CL_SqliteConnection bd_conexion("bdjuego.sql3");
		CL_DBCommand consulta_login = bd_conexion.create_command("SELECT password FROM Usuarios WHERE id=?1");
		consulta_login.set_input_parameter_string(1,idlogin);
		CL_DBReader reader = bd_conexion.execute_reader(consulta_login);
		CL_String pas_bd;

		if(reader.retrieve_row())
		{
			pas_bd = reader.get_column_string(0);

			if(password.compare(pas_bd)==0) //Password Correcto
			{
				usuario->nombre_usuario= nombre_usuario;
				usuario->id = siguiente_usuario_id++;
				Usuarios.push_back(usuario);
				usuario->enviar_evento(CL_NetGameEvent("Login-Completado"));
			}
			else //Clave Incorrecta
			{
				usuario->enviar_evento(CL_NetGameEvent("Login-Fallado", "Clave Incorrecta"));
			}
		}
		else
		{
			usuario->enviar_evento(CL_NetGameEvent("Login-Fallado", "ID No encontrado")); //ID Incorrecto
		}


		reader.close(); 
	}
	else
	{
		usuario->enviar_evento(CL_NetGameEvent("Login-ServidorLleno"));
	}
	*/
	
		CL_String idlogin = e.get_argument(0);
		CL_String password = e.get_argument(1);
		CL_String nombre_usuario = e.get_argument(2);
		usuario->nombre_usuario= nombre_usuario;
		usuario->id = siguiente_usuario_id++;
		Usuarios.push_back(usuario);
		usuario->enviar_evento(CL_NetGameEvent("Login-Completado"));

}

//Evento Juego: Solicitud de comenzar
void Servidor::on_evento_juego_solicitudcomenzar(const CL_NetGameEvent &e, UsuarioServidor *usuario)
{
	CL_String nombre = e.get_argument(0);
	cl_log_event("eventos", "Cliente solicito comenzar juego");

	CL_String mensaje= "Se conecto " + usuario->nombre_usuario;

	mundo->AgregarCadenaChat(mensaje);

	int id_nuevo = mundo->tanques.size();

	enviaratodosEx(CL_NetGameEvent("Juego-Msj",mensaje),id_nuevo); //Enviar a todos que se conecto alquien

	CL_Vec2f pospawn = mundo->agregarTanque(id_nuevo,DEFAULT,nombre,false,false); //agrega el usuario
																									//Devuelve el id_actual
	usuario->enviar_evento(CL_NetGameEvent("Juego-Comenzar",pospawn.x,pospawn.y,id_nuevo, nombre, mundo->getActualSubID())); //envia el SpawnPoint al usuario
	//enviaratodosEx(CL_NetGameEvent("Juego-AgregarTanque",pospawn.x,pospawn.y,id_nuevo, nombre), id_nuevo); //envia el SpawnPoint al usuario
	
	//Envia los tanques creados
	std::list<Tanque *>::iterator it;

	for(it=mundo->tanques.begin(); it != mundo->tanques.end(); ++it)
	{
		CL_NetGameEvent evento("Juego-JugadorConectado",(*it)->getID(),(*it)->getPos().x,(*it)->getPos().y								 //Envia al usuario todos los tanques 
											    ,(*it)->getanguloCuerpo(),(*it)->getAnguloTorreta());
		evento.add_argument((*it)->getNombre());
		//usuario->enviar_evento(evento); //actuales
		enviaratodos(evento);
	}

	mundo->quitarTodosPowerup();

	//enviaratodosEx(CL_NetGameEvent("Juego-JugadorConectado",usuario->id,pospawn.x,pospawn.y,(*final)->getanguloCuerpo() //Envia a todos el nuevo tanque
	//													   ,(*final)->getAnguloTorreta()),usuario->id);
}

void Servidor::actualiza_SubID(int id_tanque)
{
	cl_log_event("eventos", "Actualizar SubID");
	enviaratodos(CL_NetGameEvent("Juego-ActualizarSubID",mundo->getActualSubID(),id_tanque)); //actuales [todos excepto a mi]
}

void Servidor::on_evento_juego_actualizar_posiciones(const CL_NetGameEvent &e, UsuarioServidor *usuario)
{
	cl_log_event("eventos", "Cte. Actualizar Posiciones");
	int idtanque = e.get_argument(0);

	if(idtanque != mundo->getTanqueJugador()->getID())
	{
		float x = e.get_argument(1);
		float y = e.get_argument(2);
		float angulo = e.get_argument(3);
		float angulotorreta = e.get_argument(4);

		CL_Vec2f postanque = CL_Vec2f(x,y);
		
		Tanque *tank = mundo->getTanqueID(idtanque);

		if(tank != NULL && !tank->estaMuerto())
		{
			tank->setPos(postanque);
			tank->setAngulo(angulo);
			tank->setAnguloTorreta(angulotorreta);
		}
	}
}


//Evento Juego: Actualiza Posiciones
void Servidor::actualiza_posiciones()
{
	cl_log_event("eventos", "Actualizar Posiciones");

	std::list<Tanque *>::iterator it;

	for(it=mundo->tanques.begin(); it != mundo->tanques.end(); ++it)
		enviaratodos(CL_NetGameEvent("Juego-ActualizarPosiciones",(*it)->getID(),(*it)->getPos().x
																	   ,(*it)->getPos().y								 //Envia al usuario todos los tanques 
																   ,(*it)->getanguloCuerpo(),(*it)->getAnguloTorreta())); //actuales [todos excepto a mi]
}

//Evento Juego: Actualiza Posiciones
void Servidor::actualiza_saludyscore()
{
	cl_log_event("eventos", "Actualizar Salud y Score");

	std::list<Tanque *>::iterator it;

	for(it=mundo->tanques.begin(); it != mundo->tanques.end(); ++it)
		enviaratodos(CL_NetGameEvent("Juego-ActualizarSaludyScore",(*it)->getID(),(*it)->getVida(),(*it)->getArmadura()
																  ,(*it)->getScore())); //actuales [todos excepto a mi]
}

//Evento Juego: Actualiza Tiempo
void Servidor::actualiza_tiempo()
{
	cl_log_event("eventos", "Actualizar Tiempo");

	enviaratodos(CL_NetGameEvent("Juego-ActualizarTiempo", mundo->getReloj().x, mundo->getReloj().y));
}

void Servidor::matar_tanque(int id, int tiempofin)
{
	cl_log_event("eventos", "Matar Tanque");

	enviaratodos(CL_NetGameEvent("Juego-MatarTanque", id, tiempofin));
}

void Servidor::matar_misil(int id_asesino)
{
	cl_log_event("eventos", "Matar Misil");

	enviaratodos(CL_NetGameEvent("Juego-MatarMisil", id_asesino));
}

void Servidor::otorga_powerup(int id_tanque, int id_power)
{
	cl_log_event("eventos", "Otorga Powerup");

	enviaratodos(CL_NetGameEvent("Juego-OtorgaPowerup", id_tanque, id_power));
}

void Servidor::on_evento_juego_mensajechat(const CL_NetGameEvent &e, UsuarioServidor *usuario)
{
	cl_log_event("eventos", "Mensaje recibido");
	CL_String mensaje = e.get_argument(0);

	enviaratodosEx(CL_NetGameEvent("Juego-Msj",mensaje),usuario->id); //Envia a todos -1 el mensaje recibido

	mundo->AgregarCadenaChat(mensaje);
}


void Servidor::on_evento_juego_actualizar_teclado(const CL_NetGameEvent &e, UsuarioServidor *usuario)
{
		cl_log_event("eventos", "Actualizar Teclado");

		int tipo = e.get_argument(0);
		int tecla = e.get_argument(1);

		if(tipo==0) // Tecla Down
		{
			mundo->teclatanquedown(usuario->id,tecla);
		}
		else if(tipo==1) //Tecla Up
		{
			mundo->teclatanqueup(usuario->id,tecla);
		}

		enviaratodosEx(CL_NetGameEvent("Juego-ActualizarTeclado",usuario->id,tipo,tecla),usuario->id); //Envia a todos -1 el actualizar teclado
}

void Servidor::on_evento_juego_actualizar_mouse(const CL_NetGameEvent &e, UsuarioServidor *usuario)
{
	cl_log_event("eventos", "Actualizar Mouse");
	//int numarg = e.get_argument_count();
	int id_tanque = e.get_argument(0);

	mundo->teclatanquedown(id_tanque,CL_KEY_SPACE); //Dispara
	enviaratodosEx(CL_NetGameEvent("Juego-ActualizarMouse",id_tanque),usuario->id); //Envia a todos -1 el actualizar mouse 

/*	else if(numarg==2) //Mover mouse
	{
		float x=e.get_argument(0),y=e.get_argument(1);
		mundo->getTanqueID(usuario->id)->setTargetPosTorreta(CL_Vec2f(x,y));
		this->enviaratodosEx(CL_NetGameEvent("Juego-ActualizarMouse",usuario->id,x,y),usuario->id); //Envia a todos -1 el actualizar mouse 
	}	*/
}

void Servidor::enviaratodos(CL_NetGameEvent evento) 
{
	//Envia a el evento a todos los usuarios
	std::list<UsuarioServidor*>::iterator it;
	for(it = Usuarios.begin(); it!=Usuarios.end(); ++it)
	{
		(*it)->enviar_evento(evento);
	}
}


void Servidor::enviaratodosEx(CL_NetGameEvent evento,int idexcepcion) 
{
	//Envia a el evento a todos los usuarios excepto al usuario idexcepcion
	std::list<UsuarioServidor*>::iterator it;
	for(it = Usuarios.begin(); it!=Usuarios.end(); ++it)
		if((*it)->id!=idexcepcion)
		{
			(*it)->enviar_evento(evento);
		}
	
}

void Servidor::agrega_powerup(int tipo, float x, float y)
{
	cl_log_event("eventos", "Agregar Powerup");

	enviaratodos(CL_NetGameEvent("Juego-AgregaPowerup", tipo, x, y));
}