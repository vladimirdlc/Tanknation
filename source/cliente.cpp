#include "cliente.h"
#include "mundo.h"
#include "tanque.h"

class Mundo;




Cliente::Cliente(Mundo *mun)
{
	slots.connect(cliente_red.sig_event_received(), this, &Cliente::on_evento_recibido);
	slots.connect(cliente_red.sig_connected(), this, &Cliente::on_conectado);
	slots.connect(cliente_red.sig_disconnected(), this, &Cliente::on_desconectado);

	eventos_login.func_event("Login-Completado").set(this,&Cliente::on_evento_login_completado);
	eventos_login.func_event("Login-Fallado").set(this, &Cliente::on_evento_login_fallado);
	eventos_login.func_event("Login-ServidorLleno").set(this, &Cliente::on_evento_login_servidorfull);

	eventos_juego.func_event("Juego-Comenzar").set(this, &Cliente::on_evento_juego_comenzar);
	eventos_juego.func_event("Juego-Msj").set(this,&Cliente::on_evento_juego_mensajechat);
	eventos_juego.func_event("Juego-JugadorConectado").set(this,&Cliente::on_evento_juego_jugador_conectado);
	eventos_juego.func_event("Juego-JugadorDesconectado").set(this,&Cliente::on_evento_juego_jugador_desconectado);
	eventos_juego.func_event("Juego-ActualizarTeclado").set(this,&Cliente::on_evento_juego_actualizar_teclado);
	eventos_juego.func_event("Juego-ActualizarMouse").set(this,&Cliente::on_evento_juego_actualizar_mouse);
	eventos_juego.func_event("Juego-ActualizarPosiciones").set(this,&Cliente::on_evento_juego_actualizar_posiciones);
	eventos_juego.func_event("Juego-ActualizarScore").set(this,&Cliente::on_evento_juego_saludyscore);
	eventos_juego.func_event("Juego-ActualizarSubID").set(this,&Cliente::on_evento_juego_actualizar_SubID);
	eventos_juego.func_event("Juego-AgregaPowerup").set(this,&Cliente::on_evento_juego_agrega_powerup);
	eventos_juego.func_event("Juego-ActualizarSaludyScore").set(this,&Cliente::on_evento_juego_saludyscore);
	eventos_juego.func_event("Juego-ActualizarTiempo").set(this,&Cliente::on_evento_juego_actualizar_tiempo);
	eventos_juego.func_event("Juego-MatarTanque").set(this,&Cliente::on_evento_juego_matar_tanque);
	eventos_juego.func_event("Juego-MatarMisil").set(this,&Cliente::on_evento_juego_matar_misil);
	eventos_juego.func_event("Juego-OtorgaPowerup").set(this,&Cliente::on_evento_juego_otorga_powerup);
	eventos_juego.func_event("Juego-QuitaTodosPowerup").set(this,&Cliente::on_evento_juego_quitaTodosPowerup);
	eventos_juego.func_event("Juego-AgregarTanque").set(this,&Cliente::on_evento_juego_agrega_tanque);

	ip_servidor = "tanknation.servergame.org"; 
	puerto = "4556";

	idlogin = "Lolcat";
	password ="youneverwin23";

	quit = false;
	logged_in = false;

	mundo = mun;

	cl_log_event("system", "Cliente iniciado");
}

Cliente::Cliente() 
{
	
}

Cliente::~Cliente()
{
}

void Cliente::procesareventos()
{
	cliente_red.process_events();
}


//Conectar a servidor
void Cliente::conectar_a_servidor()
{
	try
	{
		cliente_red.connect(ip_servidor,puerto);
	}
	catch(const CL_Exception &e)
	{
		cl_log_event("Error", e.message);
	}
}



//Conexion satistactoria
void Cliente::on_conectado()
{
	cl_log_event("red", "Conectado al servidor");
	cliente_red.send_event(CL_NetGameEvent("Login",idlogin,password,mundo->getNombreJugador()));
}

//Desconectado del servidor
void Cliente::on_desconectado()
{
	cl_log_event("red", "Desconectado del servidor");
	quit = true;
}

void Cliente::on_evento_juego_agrega_tanque(const CL_NetGameEvent &e) 
{
	cl_log_event("eventos", "Agrega Tanque");
	float x = e.get_argument(0);  //Obtiene posicion 
	float y = e.get_argument(1);
	int mi_id = e.get_argument(2);
	CL_String nombre = e.get_argument(3);
	int contSubID = e.get_argument(4);
	CL_Vec2f postanque = CL_Vec2f(x,y);
	mundo->quitarTanque(0); //Se quita y se agrega el jugador
	mundo->agregarTanque(mi_id,DEFAULT,nombre,true,false,postanque,0.0f,0.0f);
	mundo->setSubID(contSubID);
}

//Evento Server: Recibido
void Cliente::on_evento_recibido(const CL_NetGameEvent &e) 
{
	cl_log_event("eventos", "Servidor envio evento: %1", e.to_string());

	bool evento_manejado = false;
	
	if(!logged_in)
	{
		// Usuario no se ha logeado, enrutar a eventos del despachador de login
		 evento_manejado = eventos_login.dispatch(e);
	}
	else
	{
		// Usuario logiado, enrutar a eventos del despachador del juego
		 evento_manejado = eventos_juego.dispatch(e);
	}

	if(! evento_manejado)
	{
		//Evento no manejado 
		cl_log_event("eventos", "Evento no manejado: %1", e.to_string());
	}
}

void Cliente::on_evento_login_servidorfull(const CL_NetGameEvent &e)
{
	//Servidor Lleno
	cl_log_event("eventos", "Servidor Lleno");
	
	mundo->AgregarCadenaChat("Servidor Lleno");
	cliente_red.disconnect(); //Se desconecta
}


//Evento Login: Completado
void Cliente::on_evento_login_completado(const CL_NetGameEvent &e)
{
	cl_log_event("eventos", "Login Completado");

	logged_in = true; 

	cliente_red.send_event(CL_NetGameEvent("Juego-Comenzar", mundo->getTanqueJugador()->getNombre()));
}

//Evento Login: Fallado
void Cliente::on_evento_login_fallado(const CL_NetGameEvent &e)
{
	CL_String falla = e.get_argument(0);
	cl_log_event("eventos", "Login: %1", falla); //Muestra tipo de falla
	mundo->AgregarCadenaChat("Login:" + falla);
}


//Evento Juego: Comenzar
void Cliente::on_evento_juego_comenzar(const CL_NetGameEvent &e) 
{
	int mi_id = e.get_argument(2);

	if(mundo->getTanqueID(mi_id) == NULL) //Agrega el tanque del jugador
	{
		cl_log_event("eventos", "Empieza el juego!");

		float x = e.get_argument(0);  //Obtiene posicion 
		float y = e.get_argument(1);

		CL_String nombre = e.get_argument(3);
		int contSubID = e.get_argument(4);
		CL_Vec2f postanque = CL_Vec2f(x,y);

		mundo->quitarTanque(0); //Se quita y se agrega el jugador
		mundo->agregarTanque(mi_id,DEFAULT,nombre,true,false,postanque,0.0f,0.0f);
		mundo->setSubID(contSubID);
	}

 	mundo->quitarTodosPowerup();
//mundo->agregarTanqueJugador(postanque); //Agrega el TanqueJugador de cliente
}

void Cliente::on_evento_juego_quitaTodosPowerup(const CL_NetGameEvent &e)
{
	cl_log_event("eventos", "Quita Todos Powerup");
	mundo->quitarTodosPowerup();
}

//Conexion satistactoria
void Cliente::actualiza_posiciones()
{
	Tanque *tank = mundo->getTanqueJugador();
	cliente_red.send_event(CL_NetGameEvent("Juego-ActualizarPosiciones", tank->getID(), tank->getPos().x, tank->getPos().y, tank->getanguloCuerpo(), tank->getAnguloTorreta()));
}

void Cliente::on_evento_juego_actualizar_SubID(const CL_NetGameEvent &e)
{
	cl_log_event("eventos", "Actualiza SubID");
	int contSubID = e.get_argument(0);
	int tanqueid = e.get_argument(1);
	mundo->setSubID(contSubID);
	mundo->getTanqueID(tanqueid)->disparar();
}

void Cliente::on_evento_juego_mensajechat(const CL_NetGameEvent &e)
{
	cl_log_event("eventos", "Mensaje recibido");
	CL_String mensaje = e.get_argument(0);

	mundo->AgregarCadenaChat(mensaje);	 //Agrega mensaje recibido
}

void Cliente::on_evento_juego_jugador_conectado(const CL_NetGameEvent &e)
{	
	//Datos del tanque conectado
	int idtanque = e.get_argument(0);

	if(mundo->getTanqueID(idtanque) == NULL)
	{
		float x = e.get_argument(1);
		float y = e.get_argument(2);
		float angulo = e.get_argument(3);
		float angulotorreta = e.get_argument(4);
		CL_String nombre = e.get_argument(5);

		CL_Vec2f postanque = CL_Vec2f(x,y);
		mundo->agregarTanque(idtanque,DEFAULT,nombre,false,false,postanque,angulo,angulotorreta); //Agrega el tanque nuevo	
	}
}

void Cliente::on_evento_juego_jugador_desconectado(const CL_NetGameEvent &e)
{
	int idtanque = e.get_argument(0);
	mundo->quitarTanque(idtanque); 
}

void Cliente::on_evento_juego_actualizar_posiciones(const CL_NetGameEvent &e)
{
	int idtanque = e.get_argument(0);
	//Si no soy yo o estoy muerto actualiza
	if(idtanque != mundo->getTanqueJugador()->getID() || mundo->getTanqueJugador()->getPos().x > 5000)
	{
		float x = e.get_argument(1);
		float y = e.get_argument(2);
		float angulo = e.get_argument(3);
		float angulotorreta = e.get_argument(4);

		CL_Vec2f postanque = CL_Vec2f(x,y);
		
		Tanque *tank = mundo->getTanqueID(idtanque);

		if(tank != NULL)
		{
			tank->setPos(postanque);
			tank->setAngulo(angulo);
			tank->setAnguloTorreta(angulotorreta);
		}
	}
}

void Cliente::on_evento_juego_actualizar_teclado(const CL_NetGameEvent &e)
{
		cl_log_event("eventos", "Actualizar Teclado");

		int idtanque = e.get_argument(0);

	if(idtanque != mundo->getTanqueJugador()->getID())
	{
		int tipo = e.get_argument(1);
		int tecla = e.get_argument(2);
	

		if(tipo==0) // Tecla Abajo
		{
			mundo->teclatanquedown(idtanque,tecla);
		}
		else if(tipo==1) //Tecla Arriba
		{
			mundo->teclatanqueup(idtanque,tecla);
		}
	}
		
}

void Cliente::on_evento_juego_matar_tanque(const CL_NetGameEvent &e)
{
	cl_log_event("eventos", "Matar Tanque");

	int idtanque = e.get_argument(0);
	int tiempofin = e.get_argument(1);


	mundo->getTanqueID(idtanque)->Matar(CL_System::get_time(), tiempofin);
}

void Cliente::on_evento_juego_matar_misil(const CL_NetGameEvent &e)
{
	cl_log_event("eventos", "Matar Misil");
	int id_asesino = e.get_argument(0);
	mundo->id_quitar.push_back(id_asesino);
}

void Cliente::on_evento_juego_actualizar_mouse(const CL_NetGameEvent &e)
{
	cl_log_event("eventos", "Actualizar Mouse");
	int numarg = e.get_argument_count();

	int idtanque = e.get_argument(0);

	if(idtanque != mundo->getTanqueJugador()->getID())
	{
		if(numarg==1) //Boton Izquierdo (Disparar)
		{
			mundo->teclatanquedown(idtanque,CL_KEY_SPACE);
		}
		/*else if(numarg==3) //Mover mouse
		{
			float x=e.get_argument(1),y=e.get_argument(2); //coordenadas
			mundo->getTanqueID(idtanque)->setTargetPosTorreta(CL_Vec2f(x,y));
		}*/	
	}
}

void Cliente::on_evento_juego_saludyscore(const CL_NetGameEvent &e)
{
	int idtanque = e.get_argument(0);
	int vida = e.get_argument(1);
	int armadura = e.get_argument(2);
	int score = e.get_argument(3);

	Tanque *tank = mundo->getTanqueID(idtanque);

	if(tank != NULL)
	{
		tank->setVida(vida);
		tank->setArmadura(armadura);
		tank->setScore(score);
	}
	
}

void Cliente::on_evento_juego_actualizar_tiempo(const CL_NetGameEvent &e)
{
	int min = e.get_argument(0);
	int seg = e.get_argument(1);

	mundo->setReloj(min, seg, false);
}

void Cliente::on_evento_juego_agrega_powerup(const CL_NetGameEvent &e)
{
	int i = e.get_argument(0);
	float x = e.get_argument(1);
	float y = e.get_argument(2);

	PowerupTipo tipos[4] = {MEDIKIT,ESCUDO,RAPIDEZ_TANQUE,DISPARO_360};

	mundo->agregarPowerup(tipos[i], CL_Vec2f(x, y));
}

void Cliente::on_evento_juego_otorga_powerup(const CL_NetGameEvent &e)
{
	int id_tanque = e.get_argument(0);
	int id_power = e.get_argument(1);

	mundo->otorgaPowerup(id_tanque, id_power);
}
