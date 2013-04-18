#include <ClanLib/display.h>
#include <ClanLib/sound.h>
#include <math.h>

#include "misil.h"
#include "mundo.h"
#include "tanque.h"

Misil::Misil(Mundo *mundo, Tanque *_dueño, MisilTipo tipo)
: GameObject(mundo)
{
	miTipo = tipo;
	
	CL_GraphicContext gc = mundo->get_gc();

	switch(tipo)
	{
		case DEFAULT:
		spriteMisil = new CL_Sprite(gc, "MisilNormal", mundo->recursos);
		sonido = new CL_SoundBuffer("MisilChoque", mundo->recursos);
		collisionMisil = new CL_CollisionOutline("data/Outlines/misil_normal.out");
		collisionMisil->set_alignment(origin_bottom_center);
		collisionMisil->set_rotation_hotspot(origin_bottom_center);

		sonido->set_volume(1.0f);
		sonido->prepare();
		
		sprite = spriteMisil;
		
		oculto = false;
		explotando = false;
		perseguidor = false;

		break;

		case T360:
		spriteMisil = new CL_Sprite(gc, "MisilNormal", mundo->recursos);
		spriteMisil->set_color(CL_Colorf::CL_Colorf(0.3f,0.5f,1.0f,1.0f));
		sonido = new CL_SoundBuffer("MisilChoque", mundo->recursos);
		collisionMisil = new CL_CollisionOutline("data/Outlines/misil_normal.out");
		collisionMisil->set_alignment(origin_bottom_center);
		collisionMisil->set_rotation_hotspot(origin_bottom_center);

		sonido->set_volume(1.0f);
		sonido->prepare();
		
		sprite = spriteMisil;
		
		oculto = false;
		explotando = false;
		perseguidor = false;

		break;
	}

	dueño = _dueño;
	SubID = mundo->getSubID();	
}

Misil::~Misil()
{
	delete spriteMisil;
	delete sonido;
	delete collisionMisil;
}

void Misil::setPos(CL_Vec2f _pos)
{
	Posicion = _pos;
	collisionMisil->set_translation(Posicion.x,Posicion.y);
}

void Misil::setAngulo(float _angulo)
{
	angulo = _angulo;
	sprite->set_angle(CL_Angle(angulo, cl_degrees));
	collisionMisil->set_angle(CL_Angle(angulo, cl_degrees));
}

void Misil::setVelocidad(float _velocidad)
{
	velocidad = _velocidad;
}


void Misil::setPerseguidor(Tanque *tanqueaPerseguir,int _tiempodisparo)
{
	perseguidor = true;
	tiempodisparo = _tiempodisparo;
	TargetTanque = tanqueaPerseguir;
}

void Misil::mover(float dt)
{
	if(perseguidor)
	{
		int tiempdodesdedisparo= CL_System::get_time() - tiempodisparo;

		if(tiempdodesdedisparo >= 10)
		{	
			CL_Vec2f vector = Posicion - TargetTanque->getPos();
			
			float distancia = vector.length();
			mundo->AgregarCadenaChat(cl_format("%1",distancia));

			CL_Vec2f up(0.0f, 1.0f);
			float angulodest = up.angle(vector).to_degrees();

			/*if(TargetTanque->getPos().x < Posicion.x)
				angulodest = 360.0f - angulo;*/
			/*	else
				angulodest = 360.0f  angulo;*/

			angulodelta = angulodest - angulo;

			if(distancia < 150)
			{
				if(angulodelta > 0)
					angulodelta +=5;
			}
				
			if(angulodelta > 180.0f)	
			{
				angulodelta -= 360.0f;
				angulo += 360.0f;
			}
			if(angulodelta < -180.0f)
			{
				angulodelta += 360.0f;
				angulo -= 360.0f;
			}

			angulo +=angulodelta*dt/velocidad;

			setAngulo(angulo);
		}

	}

	Posicion.x += dt* float(sin(angulo* CL_PI / 180.0f));
	Posicion.y += dt* float(-cos(angulo * CL_PI / 180.0f));
	collisionMisil->set_translation(Posicion.x, Posicion.y);
}

void Misil::draw()
{
	if(!oculto)
	{
		CL_GraphicContext gc = mundo->get_gc();
		/*CL_Pointf p	= collisionMisil->get_translation();
		collisionMisil->draw(p.x - Posicion.x,p.y - Posicion.y,CL_Colorf::greenyellow,gc);*/
		sprite->draw(mundo->get_gc(),Posicion.x,Posicion.y);
	}
}

MisilTipo Misil::getTipo()
{
	return miTipo;
}

int Misil::getDanio()
{
	switch(miTipo)
	{
		case NORMAL:
			return -10;
		case T360:
			return -40;
	}
	return -10;
}

int Misil::getSubID()
{
	return SubID;
}

bool Misil::update(float dt)
{
	if(!mundo->puedeActualizar()) //Si es cliente
	{
		//Para saber a quien asesino
		std::list<int>::iterator it;
		for(it = mundo->id_quitar.begin(); it != mundo->id_quitar.end(); ++it)
		{
			if((*it) == SubID)
			{
				mundo->id_quitar.remove(SubID);
				explotar(); //Explota este misil
				break;
			}
			/*
			else if ((*it) < SubID-10) //si es menor para borrar posibles casos no tomados
			{
				mundo->id_quitar.remove(SubID);
			}*/
		}
	}

	sprite->update((int)(dt*1000));

	if(explotando)
	{
		return false;
	}
	else
	{
		mover(velocidad*dt);
		
		int idcol = mundo->revisarColision(collisionMisil, dueño);
		
		if(idcol >= 0)
		{
		
			if(idcol < 10)
			{
				Tanque *temp = mundo->getTanqueID(idcol);

				temp->modSalud(getDanio());
				
				if(temp->estaMuerto())
				{
					mundo->getTanqueID(dueño->getID())->subirScore();
			//		temp->Matar(CL_System::get_time(),5000);
					mundo->matarTanque(temp->getID(),5000);
				}
			
					mundo->actualiza_saludyscore(); 
			}
			
			mundo->matarMisil(SubID);
			explotar();
		}
	}

	return !(Posicion.x < -100 || Posicion.y < -100 || Posicion.x > mundo->get_gc().get_width() + 100 || Posicion.y > mundo->get_gc().get_height() + 100);
}

void Misil::explotar()
{
		sonido->play();
		sprite->set_alpha(0.0f);
		explotando = true;
		//para saber donde va a eplotar
		mundo->efectoExplosion(collisionMisil->get_translation());
}
