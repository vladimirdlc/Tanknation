#include <ClanLib/core.h>
#include <ClanLib/network.h>
#include "usuario_servidor.h"

UsuarioServidor::UsuarioServidor()
: conexion(0), id(0)
{
}

void UsuarioServidor::adjuntar_conexion(CL_NetGameConnection *conexion)
{
	this->conexion = conexion;

	if(conexion)
		conexion->set_data("serveruser", this);
}

UsuarioServidor *UsuarioServidor::get_usuario(CL_NetGameConnection* conexion)
{
	if(conexion)
		return reinterpret_cast<UsuarioServidor*>(conexion->get_data("serveruser"));
	else
		return 0;
}

void UsuarioServidor::enviar_evento(const CL_NetGameEvent &eventoJuego)
{
	if(conexion)
		conexion->send_event(eventoJuego);
}
