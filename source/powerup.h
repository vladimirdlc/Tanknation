#ifndef _POWERUP_H_
#define _POWERUP_H_

#include "gameobject.h"

class CL_Sprite;

enum PowerupTipo
{
	//DROP
	MEDIKIT = 20,
	ESCUDO = 30,
	RAPIDEZ_TANQUE = 40,
	DISPARO_360 = 50,
	//RAPIDEZ_MISIL = 50,
	//PODER
	FUEGO,
	ACIDO,
	BALAS,
	BREBOTAN,
	//ESPECIAL
	ACEITE, /*resbaladizo*/
	AIRBORNE,
	HIPERSHIELD,
	NUKE,
	LOCURA
};

class Powerup : public GameObject
{

public:
	
	Powerup(PowerupTipo powerup_tipo, Mundo *mundo);
	~Powerup();
		
	PowerupTipo getTipo();

	void setPos(CL_Vec2f _pos);
	CL_Vec2f getPos();

	void setAngulo(float _angulo);

	void setTiempovivo(int _tiempo);
	int getTiempovivo();
	
	void draw();
	bool update(float timeElapsed);
    bool revisarColision(CL_CollisionOutline *outline, GameObject *otro);
	CL_CollisionOutline getCollisionPowerup();

private:

	CL_Sprite *sprite;
	CL_CollisionOutline *collisionPowerup;
	
	float angulo;	
	CL_Vec2f Posicion;
    int tiempovivo;
	
	PowerupTipo tipo;
	
	bool explotando;
};

#endif
