#pragma once

#include <ClanLib/core.h>

class CL_NetGameConnection;
class CL_NetGameEvent;

class UsuarioServidor
{
public:
	static UsuarioServidor *get_usuario(CL_NetGameConnection* conexion);

public:
	UsuarioServidor();

	void adjuntar_conexion(CL_NetGameConnection *conexion);

	void enviar_evento(const CL_NetGameEvent &juegoEvento);

public:
	int id;
	CL_String nombre_usuario;

private:
	CL_NetGameConnection *conexion;
};
