#ifndef _CONSTRUCCION_H_
#define _CONSTRUCCION_H_

#include "gameobject.h"

class CL_Sprite;
class CL_SoundBuffer;

class Construccion : public GameObject
{

public:
	enum ConstruccionTipo
	{
		HELI_PAD
	};

public:
	Construccion(ConstruccionTipo construccion_tipo, Mundo *mundo);
	~Construccion();
	
	void setPos(CL_Vec2f _pos);
	void setAngulo(float _angulo);
	
	virtual void draw();
	virtual bool update(float dt);
	virtual bool hitCheck(CL_CollisionOutline *outline, GameObject *otro);


private:
	CL_Sprite *sprite;
	CL_CollisionOutline *collisionConstruccion;
	
	float angulo;	
	CL_Vec2f Posicion;
	
	bool explotando;
};

#endif
