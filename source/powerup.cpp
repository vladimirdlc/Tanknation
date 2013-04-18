#include <ClanLib/display.h>
#include <ClanLib/sound.h>

#include "powerup.h"
#include "mundo.h"

Powerup::Powerup(PowerupTipo _tipo, Mundo *mundo)
: GameObject(mundo)
{	
	CL_GraphicContext gc = mundo->get_gc();

	tipo = _tipo;

	switch(_tipo)
	{
		case MEDIKIT:
			sprite = new CL_Sprite(gc,"Medikit",mundo->recursos);
			collisionPowerup = new CL_CollisionOutline("data/Outlines/powerup.out");
			setAngulo(0);
			collisionPowerup->set_alignment(origin_center);
			break;

		case ESCUDO:
			sprite =new CL_Sprite(gc,"Escudo",mundo->recursos);
			collisionPowerup = new CL_CollisionOutline("data/Outlines/powerup.out");
			setAngulo(0);
			collisionPowerup->set_alignment(origin_center);
			break;

		case RAPIDEZ_TANQUE:
			sprite =new CL_Sprite(gc,"RapidezTanque",mundo->recursos);
			collisionPowerup = new CL_CollisionOutline("data/Outlines/powerup.out");
			setAngulo(0);
			collisionPowerup->set_alignment(origin_center);
			break;

		case DISPARO_360:
			sprite =new CL_Sprite(gc,"RapidezMisil",mundo->recursos);
			collisionPowerup = new CL_CollisionOutline("data/Outlines/powerup.out");
			setAngulo(0);
			collisionPowerup->set_alignment(origin_center);
			break;

	}
	
	explotando = false;
}

Powerup::~Powerup()
{
	delete sprite;
	delete collisionPowerup;
}

void Powerup::setPos(CL_Vec2f _pos)
{
	Posicion = _pos;
	collisionPowerup->set_translation(Posicion.x,Posicion.y);
}

CL_Vec2f Powerup::getPos()
{
	return Posicion;
}

void Powerup::setAngulo(float _angulo)
{
	angulo = _angulo;
	sprite->set_angle(CL_Angle(angulo, cl_degrees));
	collisionPowerup->set_angle(CL_Angle(angulo, cl_degrees));
}

void Powerup::setTiempovivo(int _tiempo)
{
	tiempovivo = _tiempo;
}

int Powerup::getTiempovivo() 
{
	return tiempovivo;
}

PowerupTipo Powerup::getTipo()
{
	return tipo;
}

CL_CollisionOutline Powerup::getCollisionPowerup()
{
	return (*collisionPowerup);
}
void Powerup::draw()
{

	CL_GraphicContext gc = mundo->get_gc();

	/*CL_Pointf p = collisionPowerup->get_translation();
	collisionPowerup->draw(p.x - Posicion.x,p.y - Posicion.y,CL_Colorf::red,gc);*/
	
	sprite->draw(gc, Posicion.x,Posicion.y);
}

bool Powerup::update(float dt)
{
	sprite->update((int)(dt*1000));
	return true;
}

bool Powerup::revisarColision(CL_CollisionOutline *outline, GameObject *otro)
{
	return collisionPowerup->collide(*outline);
}
