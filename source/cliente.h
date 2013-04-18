#pragma once

#include <ClanLib/core.h>
#include <ClanLib/network.h>

class Mundo;

class Cliente 
{
public:
	Cliente();
	Cliente(Mundo *mun);
	~Cliente();
	void procesareventos();
	CL_NetGameClient cliente_red;
	void actualiza_posiciones();
	void conectar_a_servidor();
	CL_String ip_servidor;

private:

	void on_conectado();
	void on_desconectado();

	void on_evento_recibido(const CL_NetGameEvent &e);

	void on_evento_login_completado(const CL_NetGameEvent &e);
	void on_evento_login_fallado(const CL_NetGameEvent &e);
	void on_evento_login_servidorfull(const CL_NetGameEvent &e);
	void on_evento_juego_cargarmapa(const CL_NetGameEvent &e);
	void on_evento_juego_actualizar_tiempo(const CL_NetGameEvent &e);
	void on_evento_juego_saludyscore(const CL_NetGameEvent &e);
	void on_evento_juego_comenzar(const CL_NetGameEvent &e);
	void on_evento_juego_mensajechat(const CL_NetGameEvent &e);
	void on_evento_juego_jugador_conectado(const CL_NetGameEvent &e);
	void on_evento_juego_jugador_desconectado(const CL_NetGameEvent &e);
	void on_evento_juego_actualizar_posiciones(const CL_NetGameEvent &e);
	void on_evento_juego_agrega_powerup(const CL_NetGameEvent &e);
	void on_evento_juego_matar_tanque(const CL_NetGameEvent &e);
	void on_evento_juego_actualizar_teclado(const CL_NetGameEvent &e);
	void on_evento_juego_actualizar_mouse(const CL_NetGameEvent &e);
	void on_evento_juego_actualizar_SubID(const CL_NetGameEvent &e);
	void on_evento_juego_matar_misil(const CL_NetGameEvent &e);
	void on_evento_juego_otorga_powerup(const CL_NetGameEvent &e);
	void on_evento_juego_quitaTodosPowerup(const CL_NetGameEvent &e);
    void on_evento_juego_agrega_tanque(const CL_NetGameEvent &e);

private:
	CL_SlotContainer slots;

	CL_NetGameEventDispatcher_v0 eventos_login;
	CL_NetGameEventDispatcher_v0 eventos_juego;

	CL_String puerto;
	bool quit;

	bool logged_in;

	CL_String idlogin;
	CL_String password;

	Mundo *mundo;
};
