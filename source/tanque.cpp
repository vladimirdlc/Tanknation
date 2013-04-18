#include <ClanLib/display.h>
#include <ClanLib/sound.h>

#include <cstdlib> 
#include <ctime>

#include "tanque.h"
#include "mundo.h"
#include "misil.h"

#define RES_ANCHO 1024
#define RES_LARGO 768


#ifndef WIN32
// For rand()
#include <stdlib.h>
#endif

Tanque::Tanque(TanqueTipo tipo,bool _esIA, Mundo *mundo)
: GameObject(mundo)
{
	CL_GraphicContext gc = mundo->get_gc();
	miTipo = tipo;

	switch(tipo)
	{
	case DEFAULT:

		sonidoTorreta = new CL_SoundBuffer("Default_SonidoTorreta", mundo->recursos);
		sonidoTorreta->set_volume(0.5f);
		sonidoTorreta->prepare();
		
		spriteCuerpoNormal = new CL_Sprite(gc, "Default_TanqueNormal", mundo->recursos);
		spriteCuerpoMoviendose = new CL_Sprite(gc, "Default_TanqueMoviendose", mundo->recursos);
		spriteTorretaNormal = new CL_Sprite(gc, "Default_TorretaNormal", mundo->recursos);
		spriteTorretaDisparando= new CL_Sprite(gc, "Default_TorretaDisparando", mundo->recursos);
		spriteTorretaRecargando = new CL_Sprite(gc, "Default_TorretaRecargando", mundo->recursos);
		spriteTorretatFlashDisparo = new CL_Sprite(gc, "Default_TorretatFlashDisparo", mundo->recursos);
		spriteSeleccionado = new CL_Sprite(gc, "Default_TanqueSeleccionado", mundo->recursos);
		spriteBrilloRojo = new CL_Sprite(gc, "Default_BrilloRojo", mundo->recursos);

		cuerpoColision = new CL_CollisionOutline("data/Outlines/default_cuerpo_normal.out");
		cuerpoColision->set_alignment(origin_center);
		cuerpoColision->set_rotation_hotspot(origin_center);

		esIA = _esIA;

		score = 0;
			
		break;

	case BOT:

		sonidoTorreta = new CL_SoundBuffer("Default_SonidoTorreta", mundo->recursos);
		sonidoTorreta->set_volume(0.5f);
		sonidoTorreta->prepare();
		//spriteCuerpoNormal->set_color(CL_Colorf::CL_Colorf(1.0f,0.0f,0.0f,0.25f));
		//Setear el color de un sprite de esta manera representa cunato se va a dejar
		//de color de cada componente
		spriteCuerpoNormal = new CL_Sprite(gc, "Default_TanqueNormal", mundo->recursos);
		spriteCuerpoNormal->set_color(CL_Colorf::CL_Colorf(1.0f,0.8f,0.8f,1.0f));
		spriteCuerpoMoviendose = new CL_Sprite(gc, "Default_TanqueMoviendose", mundo->recursos);
		spriteCuerpoMoviendose->set_color(CL_Colorf::CL_Colorf(1.0f,0.8f,0.8f,1.0f));
		spriteTorretaNormal = new CL_Sprite(gc, "Default_TorretaNormal", mundo->recursos);
		spriteTorretaNormal->set_color(CL_Colorf::CL_Colorf(1.0f,0.8f,0.8f,1.0f));
		spriteTorretaDisparando= new CL_Sprite(gc, "Default_TorretaDisparando", mundo->recursos);
		spriteTorretaDisparando->set_color(CL_Colorf::CL_Colorf(1.0f,0.8f,0.8f,1.0f));
		spriteTorretaRecargando = new CL_Sprite(gc, "Default_TorretaRecargando", mundo->recursos);
		spriteTorretaRecargando->set_color(CL_Colorf::CL_Colorf(1.0f,0.8f,0.8f,1.0f));
		spriteTorretatFlashDisparo = new CL_Sprite(gc, "Default_TorretatFlashDisparo", mundo->recursos);
		spriteSeleccionado = new CL_Sprite(gc, "Default_TanqueSeleccionado", mundo->recursos);
		spriteBrilloRojo = new CL_Sprite(gc, "Default_BrilloRojo", mundo->recursos);

		cuerpoColision = new CL_CollisionOutline("data/Outlines/default_cuerpo_normal.out");
		cuerpoColision->set_alignment(origin_center);
		cuerpoColision->set_rotation_hotspot(origin_center);

		esIA = _esIA;

		score = 0;
			
		break;

	case BRU: //Azul ocmo el oceano, y lento. poca puntería mucho daño
		sonidoTorreta = new CL_SoundBuffer("Default_SonidoTorreta", mundo->recursos);
		sonidoTorreta->set_volume(0.5f);
		sonidoTorreta->prepare();
		//spriteCuerpoNormal->set_color(CL_Colorf::CL_Colorf(1.0f,0.0f,0.0f,0.25f));
		//Setear el color de un sprite de esta manera representa cunato se va a dejar
		//de color de cada componente
		spriteCuerpoNormal = new CL_Sprite(gc, "Default_TanqueNormal", mundo->recursos);
		spriteCuerpoNormal->set_color(CL_Colorf::CL_Colorf(0.5f,0.5f,1.0f,1.0f));
		spriteCuerpoMoviendose = new CL_Sprite(gc, "Default_TanqueMoviendose", mundo->recursos);
		spriteCuerpoMoviendose->set_color(CL_Colorf::CL_Colorf(0.5f,0.5f,1.0f,1.0f));
		spriteTorretaNormal = new CL_Sprite(gc, "Default_TorretaNormal", mundo->recursos);
		spriteTorretaNormal->set_color(CL_Colorf::CL_Colorf(0.5f,0.5f,1.0f,1.0f));
		spriteTorretaDisparando= new CL_Sprite(gc, "Default_TorretaDisparando", mundo->recursos);
		spriteTorretaDisparando->set_color(CL_Colorf::CL_Colorf(0.5f,0.5f,1.0f,1.0f));
		spriteTorretaRecargando = new CL_Sprite(gc, "Default_TorretaRecargando", mundo->recursos);
		spriteTorretaRecargando->set_color(CL_Colorf::CL_Colorf(0.5f,0.5f,1.0f,1.0f));
		spriteTorretatFlashDisparo = new CL_Sprite(gc, "Default_TorretatFlashDisparo", mundo->recursos);
		spriteSeleccionado = new CL_Sprite(gc, "Default_TanqueSeleccionado", mundo->recursos);
		spriteBrilloRojo = new CL_Sprite(gc, "Default_BrilloRojo", mundo->recursos);

		cuerpoColision = new CL_CollisionOutline("data/Outlines/default_cuerpo_normal.out");
		cuerpoColision->set_alignment(origin_center);
		cuerpoColision->set_rotation_hotspot(origin_center);

		esIA = _esIA;

		score = 0;

		break;

	case VOLDO: //Verde como la grama, y rarito. media puntería medio daño
		sonidoTorreta = new CL_SoundBuffer("Default_SonidoTorreta", mundo->recursos);
		sonidoTorreta->set_volume(0.5f);
		sonidoTorreta->prepare();
		//spriteCuerpoNormal->set_color(CL_Colorf::CL_Colorf(1.0f,0.0f,0.0f,0.25f));
		//Setear el color de un sprite de esta manera representa cunato se va a dejar
		//de color de cada componente
		spriteCuerpoNormal = new CL_Sprite(gc, "Default_TanqueNormal", mundo->recursos);
		spriteCuerpoNormal->set_color(CL_Colorf::CL_Colorf(0.5f,1.0f,0.5f,1.0f));
		spriteCuerpoMoviendose = new CL_Sprite(gc, "Default_TanqueMoviendose", mundo->recursos);
		spriteCuerpoMoviendose->set_color(CL_Colorf::CL_Colorf(0.5f,1.0f,0.5f,1.0f));
		spriteTorretaNormal = new CL_Sprite(gc, "Default_TorretaNormal", mundo->recursos);
		spriteTorretaNormal->set_color(CL_Colorf::CL_Colorf(0.5f,1.0f,0.5f,1.0f));
		spriteTorretaDisparando= new CL_Sprite(gc, "Default_TorretaDisparando", mundo->recursos);
		spriteTorretaDisparando->set_color(CL_Colorf::CL_Colorf(0.5f,1.0f,0.5f,1.0f));
		spriteTorretaRecargando = new CL_Sprite(gc, "Default_TorretaRecargando", mundo->recursos);
		spriteTorretaRecargando->set_color(CL_Colorf::CL_Colorf(0.5f,1.0f,0.5f,1.0f));
		spriteTorretatFlashDisparo = new CL_Sprite(gc, "Default_TorretatFlashDisparo", mundo->recursos);
		spriteSeleccionado = new CL_Sprite(gc, "Default_TanqueSeleccionado", mundo->recursos);
		spriteBrilloRojo = new CL_Sprite(gc, "Default_BrilloRojo", mundo->recursos);

		cuerpoColision = new CL_CollisionOutline("data/Outlines/default_cuerpo_normal.out");
		cuerpoColision->set_alignment(origin_center);
		cuerpoColision->set_rotation_hotspot(origin_center);

		esIA = _esIA;

		score = 0;

		break;


	case ROSA: //Verde como la grama, y rarito. media puntería medio daño
		sonidoTorreta = new CL_SoundBuffer("Default_SonidoTorreta", mundo->recursos);
		sonidoTorreta->set_volume(0.5f);
		sonidoTorreta->prepare();
		//spriteCuerpoNormal->set_color(CL_Colorf::CL_Colorf(1.0f,0.0f,0.0f,0.25f));
		//Setear el color de un sprite de esta manera representa cunato se va a dejar
		//de color de cada componente
		spriteCuerpoNormal = new CL_Sprite(gc, "Default_TanqueNormal", mundo->recursos);
		spriteCuerpoNormal->set_color(CL_Colorf::CL_Colorf(0.9f,0.5f,0.5f,0.2f));
		spriteCuerpoMoviendose = new CL_Sprite(gc, "Default_TanqueMoviendose", mundo->recursos);
		spriteCuerpoMoviendose->set_color(CL_Colorf::CL_Colorf(0.9f,0.5f,0.5f,0.2f));
		spriteTorretaNormal = new CL_Sprite(gc, "Default_TorretaNormal", mundo->recursos);
		spriteTorretaNormal->set_color(CL_Colorf::CL_Colorf(0.9f,0.5f,0.5f,0.2f));
		spriteTorretaDisparando= new CL_Sprite(gc, "Default_TorretaDisparando", mundo->recursos);
		spriteTorretaDisparando->set_color(CL_Colorf::CL_Colorf(0.9f,0.5f,0.5f,0.2f));
		spriteTorretaRecargando = new CL_Sprite(gc, "Default_TorretaRecargando", mundo->recursos);
		spriteTorretaRecargando->set_color(CL_Colorf::CL_Colorf(0.9f,0.5f,0.5f,0.2f));
		spriteTorretatFlashDisparo = new CL_Sprite(gc, "Default_TorretatFlashDisparo", mundo->recursos);
		spriteSeleccionado = new CL_Sprite(gc, "Default_TanqueSeleccionado", mundo->recursos);
		spriteBrilloRojo = new CL_Sprite(gc, "Default_BrilloRojo", mundo->recursos);

		cuerpoColision = new CL_CollisionOutline("data/Outlines/default_cuerpo_normal.out");
		cuerpoColision->set_alignment(origin_center);
		cuerpoColision->set_rotation_hotspot(origin_center);

		esIA = _esIA;

		score = 0;

		break;

	}
	
	anguloCuerpo= anguloTorreta = anguloDestCuerpo = anguloDeltaCuerpo =  anguloDestTorreta = anguloDeltaTorreta= 0.0f;
	seMovAdelante = seMovAtras = seMovDer = seMovIzq = false;

	seleccionado = false;
	estaDisparando = false;
	
	spriteCuerpo = spriteCuerpoNormal;
	spriteTorreta = spriteTorretaNormal;

	setParametros(tipo);	
}
//Setea los parámetros para cada tipo de tanque y les da vida quitando poderes
void Tanque::setParametros(TanqueTipo tipo) 
{
	estamuerto = false;
	tienePowerup = false;

	switch(tipo)
	{
		case DEFAULT: 
		velocidadGiroCuerpo = 130.0f;
		velocidadGiroTorreta = 150.0f;
		velocidad = 120.0f;
		setSalud(60, 0, 100, 100);
		break;
		case BOT: 
		velocidadGiroCuerpo = 80.0f;
		velocidadGiroTorreta = 150.0f;
		velocidad = 90.0f;
		setSalud(60, 0, 100, 100);
		break;
		case BRU: 
		velocidadGiroCuerpo = 70.0f;
		velocidadGiroTorreta = 140.0f;
		velocidad = 80.0f;
		setSalud(60, 0, 100, 100);
		break;
		case VOLDO: 
		velocidadGiroCuerpo = 80.0f;
		velocidadGiroTorreta = 150.0f;
		velocidad = 90.0f;
		setSalud(60, 0, 100, 100);
		break;	
		case ROSA: 
		velocidadGiroCuerpo = 90.0f;
		velocidadGiroTorreta = 160.0f;
		velocidad = 90.0f;
		setSalud(60, 0, 100, 100);
		break;	
	}
}

// Seleccionar
bool Tanque::estaSeleccionado() const
{
	return seleccionado;
}

void Tanque::seleccionar()
{
	seleccionado = true;
}

void Tanque::deseleccionar()
{
	seleccionado = false;
}

// Score

int Tanque::getScore()
{
	return score;
}

void Tanque::setScore(int record)
{
	score = record;
}

void Tanque::subirScore()
{
	score++;
}


// Movimiento
void Tanque::movAdelante()
{
	if(!seMovAtras) 
		seMovAdelante = true;
}

void Tanque::movAtras()
{
	if(!seMovAdelante)
		seMovAtras = true;
}

void Tanque::movDer()
{
	if(!seMovIzq)
		seMovDer = true;
}

void Tanque::movIzq()
{
	if(!seMovDer)
		seMovIzq = true;
}

void Tanque::noMovAdelante()
{
	seMovAdelante = false;	
}

void Tanque::noMovAtras()
{
	seMovAtras = false;
}
void Tanque::noMovDer()
{
	seMovDer = false;
}
void Tanque::noMovIzq()
{
	seMovIzq = false;
}

// Muerte
bool Tanque::estaMuerto()
{
	return estamuerto;
}

void Tanque::setEstaMuerto(bool estam)
{
	estamuerto=estam;
}

// Posicion
CL_Vec2f Tanque::getPos()
{
	return Posicion;
}

void Tanque::setPos(CL_Vec2f _pos)
{
	Posicion = PosDest = _pos;
	cuerpoColision->set_translation(_pos.x,_pos.y);
}

// Angulo Cuerpo y Torreta

float Tanque::getanguloCuerpo()
{
	return anguloCuerpo;
}

float Tanque::getAnguloTorreta()
{
	return anguloTorreta;
}



// Salud
int Tanque::getVida()
{	
	return vida;
}

void Tanque::setVida(int _vida)
{
	vida = _vida;
	if (vida >= maxvida) 
		vida = maxvida;
}

int Tanque::getArmadura()
{
	return armadura;
}

void Tanque::setArmadura(int _armadura)
{
	armadura = _armadura;
	if (armadura >= maxarmadura) 
		armadura = maxarmadura;
}

int Tanque::getMaxVida()
{
	return maxvida;
}

void Tanque::setMaxVida(int _mvida)
{
	maxvida = _mvida;
}

int Tanque::getMaxArmadura()
{
	return maxarmadura;
}

void Tanque::setMaxArmadura(int _marmadura)
{
	maxarmadura = _marmadura;
}

void Tanque::setNombre(CL_String _nombre)
{
	if(_nombre.length() < 10)
	{
		while(_nombre.length() < 10)
		{
			_nombre += " ";
		}
	}

	_nombre = _nombre.substr(0,10);
		
	nombre = _nombre;
}


CL_String Tanque::getNombre()
{
	return nombre;
}

TanqueTipo Tanque::getTipo()
{
	return miTipo;
}

void Tanque::setSalud(int vida, int armadura, int mvida, int marmadura)
{
	setMaxVida(mvida); 
	setMaxArmadura(marmadura); 
	setVida(vida); 
	setArmadura(armadura); 
	
}

bool Tanque::modSalud(int valor) // Modificar al nuevo
{ 
	if(!mundo->puedeActualizar()) //Si no puede actualizar ya que no es Server
		return false;

	int valVida=vida;

	if (valor > 0)
	{
		valVida += valor;
		if (valVida >= maxvida)     
		{
	        return false;
	    }
		else
		{
			vida = valVida;
		}
	}
	else 
    {
		int valArmadura = armadura;
		if (valArmadura > 0) 
		{
			valArmadura += valor;
			if (valArmadura < 0)
			{
				valVida += valArmadura; 
				valArmadura = 0;
			}

			valVida += (valor / 2);
			armadura = valArmadura;
		}
        else
        {
			valVida += valor;
			if(valVida < 0) 
			{
				valVida=0;

				if(!estamuerto)
				{
					Matar(CL_System::get_time(),5000);
				}
				
			}
			else
			{
				vida = valVida;
			}
        }		   
	}

	return true;
}

// Movimiento
void Tanque::avanzar()
{
	float paso = 3*velocidad/100;
	CL_GraphicContext gc = mundo->get_gc();

	CL_Angle angulo;
	angulo.set_degrees(anguloCuerpo);

	PosDest.x = Posicion.x + paso * sin(angulo.to_radians());
	if(PosDest.x < 0 || PosDest.x > RES_ANCHO /*|| mundo->revisarPosicion(PosDest, this->id)*/) PosDest.x = Posicion.x;
	
	PosDest.y = Posicion.y - paso * cos(angulo.to_radians());	
	if(PosDest.y < 0 || PosDest.y > RES_LARGO /*|| mundo->revisarPosicion(PosDest, this->id)*/) PosDest.y = Posicion.y;	

	CL_Vec2f vector = Posicion - PosDest;
	vector.normalize();
	PosDelta = -vector;

	spriteCuerpo = spriteCuerpoMoviendose;
}

void Tanque::retroceder()
{
	float paso = 3*velocidad/100;

	CL_Angle angulo;
	angulo.set_degrees(anguloCuerpo);
	
	PosDest.x = Posicion.x - paso * sin(angulo.to_radians());
	if(PosDest.x < 0 || PosDest.x > RES_ANCHO) PosDest.x = Posicion.x;

	PosDest.y = Posicion.y + paso * cos(angulo.to_radians());	
	if(PosDest.y < 0 || PosDest.y > RES_LARGO) PosDest.y = Posicion.y;	

	CL_Vec2f vector = Posicion - PosDest;
	vector.normalize();
	PosDelta = -vector;

	spriteCuerpo = spriteCuerpoMoviendose;
}

void Tanque::giroDerecha()
{
	float angulo = anguloCuerpo + 1;
	if(angulo>360) angulo -=360;
	setAnguloDest(angulo);
}

void Tanque::giroIzquierda()
{
	float angulo = anguloCuerpo - 1;
	if(angulo<0) angulo +=360;
	setAnguloDest(angulo);
}

// Target Pos Cuerpo Torreta

void Tanque::setTargetPosTorreta(CL_Vec2f _pos)
{
	CL_Vec2f vector = Posicion - _pos;
	CL_Vec2f up(0.0f, 1.0f);

	float angulo = up.angle(vector).to_degrees();
	if(_pos.x < Posicion.x)
		angulo = 360.0f - angulo;
	
	
	setAnguloDestTorreta(angulo);
}


void Tanque::setTargetPos(CL_Vec2f _pos)
{
	PosDest = _pos;

	CL_Vec2f vector = Posicion - PosDest;
	CL_Vec2f up(0.0f, 1.0f);

	float angulo = up.angle(vector).to_degrees();
	if(_pos.x < Posicion.x)
		angulo = 360.0f - angulo;

	setAnguloDest(angulo);
	
	vector.normalize();
	PosDelta = -vector;
	spriteCuerpo = spriteCuerpoMoviendose;
}

// Angulos

void Tanque::setAngulo(float _angulo)
{
	anguloCuerpo = _angulo;
	cuerpoColision->set_angle(CL_Angle(_angulo, cl_degrees));
}

void Tanque::setAnguloTorreta(float _angulo)
{
	anguloTorreta = _angulo;
}

void Tanque::setAnguloDest(float _angulodest)
{

	anguloDestCuerpo = _angulodest;
	anguloDeltaCuerpo = anguloDestCuerpo - anguloCuerpo;
	
	if(anguloDeltaCuerpo > 180.0f)
	{
		anguloDeltaCuerpo -= 360.0f;
		anguloCuerpo += 360.0f;
	}
	if(anguloDeltaCuerpo < -180.0f)
	{
		anguloDeltaCuerpo += 360.0f;
		anguloCuerpo -= 360.0f;
	}
}

void Tanque::setAnguloDestTorreta(float _angulodest)
{
	anguloDestTorreta = _angulodest;
	anguloDeltaTorreta = anguloDestTorreta - anguloTorreta;
	
	if(anguloDeltaTorreta > 180.0f)
	{
		anguloDeltaTorreta -= 360.0f;
		anguloTorreta += 360.0f;
	}
	if(anguloDeltaTorreta < -180.0f)
	{
		anguloDeltaTorreta += 360.0f;
		anguloTorreta -= 360.0f;
	}
}

CL_CollisionOutline Tanque::getCuerpoCollision()
{
	return (*cuerpoColision);
}


bool Tanque::revisarColision(CL_CollisionOutline *outline, GameObject *otro)
{
	return cuerpoColision->collide(*outline);
}

void Tanque::setEstaDisparando(bool estado)
{
	estaDisparando = estado;
}

void Tanque::disparar()
{
	if(!estaDisparando)
	{
		mundo->actualiza_SubID(id);
 		spriteTorreta = spriteTorretaDisparando;
		spriteTorretaDisparando->restart();
	
		sonidoTorreta->play();
		
		estaDisparando = true;
		
		Misil *misil = new Misil(mundo, this, NORMAL);
		misil->setPos(Posicion);
		misil->setVelocidad(500.0f);
		misil->setAngulo(anguloTorreta);
		//misil->setPerseguidor(mundo->getTanquePosMin(this),CL_System::get_time());
		misil->mover(43);
		misil->setID(getID() + 10);
		mundo->agregarObjeto(misil);
	}
}



void Tanque::disparo360()
{
		int delta = 20;
			
		for(int i=0; i<360; i += delta)
		{
			Misil *misil = new Misil(mundo, this, T360);
			misil->setPos(Posicion);
			misil->setVelocidad(500.0f);
			misil->setAngulo(anguloTorreta+i);
			misil->mover(43);
			misil->setID(getID() + 10);
			mundo->agregarObjeto(misil);
		}
}

void Tanque::hiperVelocidad()
{
	  tiempoagarroPowerup = CL_System::get_time();
	  tipoPowerup = 40;
	  velocidad = 200.0f;
	  tienePowerup = true;
}

void Tanque::Matar(int tiempoahora,int demora) 
{
	estamuerto = true;
	mundo->efectoExplosion(CL_Pointf(this->Posicion));
	setPos(CL_Vec2f(10000,10000));
	demoramuerte = demora;
	horamuerte = tiempoahora;
}

bool Tanque::update(float dt)
{
	//Esta Muerto?
	
	if(estamuerto)
	{
		tienePowerup = false;
		int tiempomuerto= CL_System::get_time() - horamuerte;

		if(tiempomuerto >= demoramuerte)
		{			
			setParametros(this->miTipo);
			if(mundo->puedeActualizar())
				setPos(mundo->getPosicionSpawn());
		}
	}
	else
	{
			spriteCuerpo->update((int)(dt*1000));
			spriteTorreta->update((int)(dt*1000));

			if(tienePowerup)
			{
				int tiempoconpowerup = CL_System::get_time() - tiempoagarroPowerup;

				if(tiempoconpowerup >= 10000)
				{
					switch (tipoPowerup)
					{
						case RAPIDEZ_TANQUE: velocidad = 170.0f; break;
					}
				}

			}

		if(spriteTorretaDisparando->is_finished() && spriteTorreta == spriteTorretaDisparando)
		{
			spriteTorreta = spriteTorretaRecargando;
			spriteTorretaRecargando->restart();
		}
		else if(spriteTorretaRecargando->is_finished() && spriteTorreta == spriteTorretaRecargando)
		{
			spriteTorreta = spriteTorretaNormal;
			spriteTorretaNormal->restart();

			estaDisparando = false;
		}
	
		//Movimiento
		if(seMovDer)
		{
			giroDerecha();
		}
		else if(seMovIzq)
		{
			giroIzquierda();
		}
		if(seMovAdelante)
		{
			Tanque::avanzar();
		}
		else if(seMovAtras)
		{
			retroceder();
		}	

		// Calculo de Angulos

		if(anguloDeltaCuerpo)
		{
			if(anguloDeltaCuerpo > 0.0f)
			{
				anguloCuerpo += velocidadGiroCuerpo * dt;
				if(anguloCuerpo > anguloDestCuerpo)
				{
					anguloCuerpo = anguloDestCuerpo;
					anguloDeltaCuerpo = 0.0f;
				}
			}
			else
			{
				anguloCuerpo -= velocidadGiroCuerpo * dt;
				if(anguloCuerpo < anguloDestCuerpo)
				{
					anguloCuerpo = anguloDestCuerpo;
					anguloDeltaCuerpo = 0.0f;
				}
			}
		}
		
		if(anguloDeltaTorreta)
		{
			if(anguloDeltaTorreta > 0.0f)
			{
				anguloTorreta += velocidadGiroTorreta * dt;
				if(anguloTorreta > anguloDestTorreta)
				{
					anguloTorreta = anguloDestTorreta;
					anguloDeltaTorreta = 0.0f;
				}
			}
			else
			{
				anguloTorreta -= velocidadGiroTorreta * dt;
				if(anguloTorreta < anguloDestTorreta)
				{
					anguloTorreta = anguloDestTorreta;
					anguloDeltaTorreta = 0.0f;
				}
			}
		}

		bool colisionmov = false;
		int idcol = mundo->revisarColision(cuerpoColision,this);

		if(idcol >= 0)
		{
				if(idcol < 20)  // con algun tanque
				{
					if(!esIA) //Colisiona un jugador se detienen
					{
						if(seMovAdelante)
						{
							seMovAdelante = false;
							Tanque::retroceder();
							Tanque::retroceder();
						}
						else if(seMovAtras)
						{
							seMovAtras = false;
							Tanque::avanzar();
						}	
					}
					else
					{
						colisionmov = true;
					}
				}
				else if(mundo->puedeActualizar())
				{
					if(idcol < 30) //medikit
					{
						if(modSalud(20)) //si lo puede tomar
						{
							//mundo->quitarPowerup(idcol);
							mundo->otorgaPowerup(id, idcol);
							mundo->actualiza_saludyscore(); 
						}
					}
					else if (idcol < 40) //escudo
					{
						if((armadura != maxarmadura))
						{ 
						  armadura += 20;
						  if(armadura > maxarmadura)
						 {
						  armadura = maxarmadura; }	
						  //mundo->quitarPowerup(idcol);
						  mundo->otorgaPowerup(id, idcol);
						  mundo->actualiza_saludyscore(); 
						}
					}
					else if (idcol < 50) //velocidad_tanque
					{
							  hiperVelocidad();
						//	  mundo->quitarPowerup(idcol);
							  mundo->otorgaPowerup(id, idcol);
					}
					else if (idcol < 60) //disparo_360
					{
							disparo360();
							//mundo->quitarPowerup(idcol);
							mundo->otorgaPowerup(id, idcol);
					}
				}
		}

		
		//IA----------------------

		if(esIA) 
		{
			TargetPosTorretaIA =  mundo->getPosMinTanque(this);

			if(colisionmov) //Se pegó con un tanque u otro obj colisionable
			{				
				TargetPosIA = CL_Vec2f(
					(getPos().x+cos((CL_PI/180.0f)*(anguloCuerpo+90+int(rand()%60)-30))*30),
					(getPos().y+sin((CL_PI/180.0f)*(anguloCuerpo+90+int(rand()%60)-30))*30));

			}
			else
			{
			if(vida < 50)
			{
				CL_Vec2f temp = mundo->getPowerupPosMin(MEDIKIT,this); 

				if(temp != CL_Vec2f(0.0f,0.0f))
					TargetPosIA =  temp;
			}
			else if(armadura < 50)
			{
				CL_Vec2f temp =mundo->getPowerupPosMin(ESCUDO,this); 

				if(temp != CL_Vec2f(0.0f,0.0f))
					TargetPosIA =  temp;
			}

			if(score < 5)
			{
				CL_Vec2f temp =mundo->getPowerupPosMin(RAPIDEZ_TANQUE,this); 

				if(temp != CL_Vec2f(0.0f,0.0f))
					TargetPosIA =  temp;
			}
			}
		
			float dist = TargetPosIA.distance(Posicion);
			if(dist < 20)
			{
				if(!mundo->revisarPosicion(TargetPosIA,this->id)) //si  hay alguien
				{
					TargetPosIA = mundo->getPosicionSpawn();
				}

			}

			setTargetPosTorreta(TargetPosTorretaIA);
			CL_Vec2f vector = Posicion - TargetPosIA;
			CL_Vec2f up(0.0f, 1.0f);

			float angulocuerpo = up.angle(vector).to_degrees();

			if(TargetPosIA.x < Posicion.x)
				angulocuerpo = 360.0f - angulocuerpo;

			setAnguloDest(angulocuerpo);

			float angulodelta =anguloDestTorreta-anguloTorreta;
			if((abs(angulodelta) < 2)) //Apuntar
			{	
				srand((unsigned)time(0));

				float angulonormal = anguloTorreta;

				int punteria;

				if(velocidad > 100)
				{
					punteria = 0;
				}
				else
				{
					punteria = -4 + rand()%4;
				}

				anguloTorreta+=punteria;

				//Si estoy sólo NO dispara
				if(!mundo->estoySolo(this)) disparar(); 

				anguloTorreta = angulonormal;
			}

			if(anguloDestCuerpo==anguloCuerpo)
			{
					
					CL_Vec2f temp = Posicion - TargetPosIA;

					if(abs(temp.x) > 2|| abs(temp.y) > 2)	
					{
						CL_Vec2f velocidadtemp = TargetPosIA - Posicion;
						velocidadtemp.normalize();
						//velocidadtemp*=1.5;//VELOCIDAD IA
						//velocidadtemp*=0.25;

						Posicion.x += velocidadtemp.x*dt*velocidad;
						Posicion.y += velocidadtemp.y*dt*velocidad;
					}
					else
					{
						TargetPosIA = mundo->getPosicionSpawn();
					}
			}
	
		} //Final IA

		if(PosDest.x != Posicion.x || PosDest.y != Posicion.y)
		{
			Posicion.x += PosDelta.x * velocidad * dt;
			Posicion.y += PosDelta.y * velocidad * dt;
			cuerpoColision->set_translation(Posicion.x, Posicion.y);
			
			if((PosDelta.x > 0 && Posicion.x > PosDest.x) || (PosDelta.x < 0 && Posicion.x < PosDest.x))
				Posicion.x = PosDest.x;
			if((PosDelta.y > 0 && Posicion.y > PosDest.y) || (PosDelta.y < 0 && Posicion.y < PosDest.y))
				Posicion.y = PosDest.y;

			if(Posicion== PosDest)
			{
				spriteCuerpo = spriteCuerpoNormal;
			}
		}
		
		
		spriteCuerpo->set_angle(CL_Angle(anguloCuerpo, cl_degrees));
		cuerpoColision->set_angle(CL_Angle(anguloCuerpo, cl_degrees));

		spriteSeleccionado->set_angle(CL_Angle(anguloCuerpo, cl_degrees));
		spriteTorreta->set_angle(CL_Angle(anguloTorreta, cl_degrees));
		spriteTorretatFlashDisparo->set_angle(CL_Angle(anguloTorreta, cl_degrees));

		cuerpoColision->set_translation(Posicion.x,Posicion.y);
		}
	
	return true;
}

void Tanque::draw()
{
	CL_GraphicContext gc = mundo->get_gc();
	
	//CL_Pointf p = cuerpoColision->get_translation();
	//cuerpoColision->draw(p.x-Posicion.x,p.y -Posicion.y,CL_Colorf::blue,gc);

	if(seleccionado)
		spriteSeleccionado->draw(gc, Posicion.x, Posicion.y);

	CL_BlendMode blend;
	blend.set_blend_function(cl_blend_zero, cl_blend_one_minus_src_alpha, cl_blend_zero, cl_blend_one_minus_src_alpha);
	gc.set_blend_mode(blend);
	spriteCuerpo->set_alpha(0.5f);
	spriteCuerpo->draw(gc, Posicion.x + 5, Posicion.y + 5);

	blend.set_blend_function(cl_blend_src_alpha, cl_blend_one_minus_src_alpha, cl_blend_src_alpha, cl_blend_one_minus_src_alpha);
	gc.set_blend_mode(blend);
	spriteCuerpo->set_alpha(1.0f);
	spriteCuerpo->draw(gc, Posicion.x, Posicion.y);
	
	blend.set_blend_function(cl_blend_zero, cl_blend_one_minus_src_alpha, cl_blend_zero, cl_blend_one_minus_src_alpha);
	gc.set_blend_mode(blend);
	spriteTorreta->set_alpha(0.5f);
	spriteTorreta->draw(gc, Posicion.x + 5, Posicion.y + 5);

	blend.set_blend_function(cl_blend_src_alpha, cl_blend_one_minus_src_alpha, cl_blend_src_alpha, cl_blend_one_minus_src_alpha);
	gc.set_blend_mode(blend);
	spriteTorreta->set_alpha(1.0f);
	spriteTorreta->draw(gc, Posicion.x, Posicion.y);

	CL_BlendMode default_blend_mode;
	gc.set_blend_mode(default_blend_mode);

	if(spriteTorreta == spriteTorretaDisparando)
	{
		spriteTorretatFlashDisparo->draw(gc, Posicion.x, Posicion.y);
	}
	
	spriteBrilloRojo->draw(gc, Posicion.x, Posicion.y);

}


