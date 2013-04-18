#pragma once

//Clanlib
#include <ClanLib/core.h>
#include <ClanLib/network.h>
#include <ClanLib/database.h>
#include <ClanLib/sqlite.h>

//C++
#include <list>

using namespace std;


class UsuarioServidor;
class Mundo;

class Servidor 
{
public:
	Servidor();
	Servidor(Mundo *mun,int njugadores);
	~Servidor();
	void procesareventos();
	void enviaratodos(CL_NetGameEvent evento);
	void enviaratodosEx(CL_NetGameEvent evento,int idexcepcion);
	void agrega_powerup(int tipo, float x, float y);
	void otorga_powerup(int id_tanque, int id_power);
	void actualiza_posiciones();
	void actualiza_saludyscore();
	void actualiza_tiempo();
	void actualiza_SubID(int id_tanque);
	void quitaTodosPowerup();
	void matar_tanque(int id, int tiempofin);
	void matar_misil(int id_asesino);
	void parar();
	CL_NetGameServer servidor_red;

private:
	void on_cliente_conectado(CL_NetGameConnection *conexion);
	void on_cliente_desconectado(CL_NetGameConnection *conexion);

	void on_evento_recibido(CL_NetGameConnection *conexion, const CL_NetGameEvent &e);

	void on_evento_login(const CL_NetGameEvent &e, UsuarioServidor *usuario);
	void on_evento_juego_solicitudcomenzar(const CL_NetGameEvent &e, UsuarioServidor *usuario);
	void on_evento_juego_mensajechat(const CL_NetGameEvent &e, UsuarioServidor *usuario);
	void on_evento_juego_actualizar_teclado(const CL_NetGameEvent &e, UsuarioServidor *usuario);
	void on_evento_juego_actualizar_mouse(const CL_NetGameEvent &e, UsuarioServidor *usuario);
	void on_evento_juego_actualizar_posiciones(const CL_NetGameEvent &e, UsuarioServidor *usuario);

private:

	CL_SlotContainer slots;

	list<UsuarioServidor*> Usuarios;

	int numjugadores;
	int siguiente_usuario_id;

	CL_NetGameEventDispatcher_v1<UsuarioServidor*> eventos_login;
	CL_NetGameEventDispatcher_v1<UsuarioServidor*> eventos_juego;

	CL_String puerto;	

	Mundo *mundo;
};
