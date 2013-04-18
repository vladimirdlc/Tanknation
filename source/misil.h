#ifndef _MISIL_H_
#define _MISIL_H_

#include "gameobject.h"

class Tanque;
class CL_Sprite;
class CL_SoundBuffer;

enum MisilTipo
{
	NORMAL,
	T360
};



class Misil : public GameObject
{

public:

	Misil(Mundo *mundo, Tanque *_dueño, MisilTipo tipo);
	~Misil();

	void setPos(CL_Vec2f _pos);
	void setAngulo(float _angulo);
	void setVelocidad(float _velocidad);
	void mover(float dt);
	void setPerseguidor(Tanque *tanqueaPerseguir,int _tiempodisparo);
	void explotar();
	int getSubID();
	MisilTipo getTipo();
	int getDanio();
	virtual void draw();
	virtual bool update(float dt);

private:

	CL_Sprite *spriteMisil;
	CL_SoundBuffer *sonido;
	
	CL_Sprite *sprite;
	CL_CollisionOutline *collisionMisil;
	
	GameObject *dueño;
	
	float angulo;
	float velocidad;
	
	CL_Vec2f Posicion;
	
	bool oculto;
	bool explotando;

	bool perseguidor;
	int tiempodisparo;
	int SubID;
	Tanque *TargetTanque;

	float angulodelta;
	MisilTipo miTipo;
};

#endif
