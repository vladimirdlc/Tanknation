//ClanLib
#include <ClanLib/display.h>
#include <ClanLib/sound.h>

#include "construccion.h"
#include "mundo.h"

Construccion::Construccion(ConstruccionTipo construccion_tipo, Mundo *mundo)
: GameObject(mundo)
{
	CL_GraphicContext gc = mundo->get_gc();

	switch(construccion_tipo)
	{
		case HELI_PAD:
			sprite = new CL_Sprite(gc, "Helipad", mundo->recursos);
			collisionConstruccion = new CL_CollisionOutline("data/Outlines/helipad.out");
			collisionConstruccion->set_alignment(origin_center);
			break;
	}
}

Construccion::~Construccion()
{
	delete sprite;
	delete collisionConstruccion;
}

void Construccion::setPos(CL_Vec2f _pos)
{
	Posicion = _pos;
	collisionConstruccion->set_translation(Posicion.x, Posicion.y);
}

void Construccion::setAngulo(float _angulo)
{
	angulo = _angulo;
	sprite->set_angle(CL_Angle(angulo, cl_degrees));
	collisionConstruccion->set_angle(CL_Angle(angulo, cl_degrees));
}

void Construccion::draw()
{
	sprite->draw(mundo->get_gc(), Posicion.x,Posicion.y);
}

bool Construccion::update(float dt)
{
	sprite->update((int)(dt*1000));
	return true;
}

bool Construccion::hitCheck(CL_CollisionOutline *outline, GameObject *otro)
{
	return collisionConstruccion->collide(*outline);
}
