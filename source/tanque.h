#ifndef _Tanque_H_
#define _Tanque_H_

#include "gameobject.h"

class CL_Sprite;
class CL_SoundBuffer;
class Mundo;

enum TanqueTipo
{
	DEFAULT,
	BOT,
	BRU,
	VOLDO,
	ROSA
};


class Tanque : public GameObject
{

public:

	Tanque(TanqueTipo tipo,bool _esIA, Mundo *mundo);

	bool estaSeleccionado() const;

	float getanguloCuerpo();

	void setAngulo(float _angulo);
	void setAnguloDest(float _angulodest);

	void  setAnguloTorreta(float _angulo);
	float getAnguloTorreta();

	void setAnguloDestTorreta(float _angulodest);

	void setTargetPosTorreta(CL_Vec2f _pos);
	
	CL_Vec2f getPos();
	void setPos(CL_Vec2f _pos);

	void setTargetPos(CL_Vec2f _pos);

	int  getVida();
	void setVida(int _vida);

	int  getArmadura();
	void setArmadura(int _armadura);

	int  getMaxVida();
	void setMaxVida(int _mvida);

	int  getMaxArmadura();
	void setMaxArmadura(int _maxarmadura);

	void setSalud(int _vida, int _armadura, int _mvida, int _marmadura);

	bool modSalud(int _valor);

	void subirScore();
	int  getScore();
	void  setScore(int record);

	void setEstaMuerto(bool _estam);
	void Matar(int _tiempoahora,int _demora);
	void hiperVelocidad();
	bool estaMuerto();

	void movAdelante();
	void movAtras();
	void movDer();
	void movIzq();
	void noMovAdelante();
	void noMovAtras();
	void noMovDer();
	void noMovIzq();
	void avanzar();
	void retroceder();
	void giroDerecha();
	void giroIzquierda();

	bool estaMovAde() const;
	bool estaMovAtr() const;
	bool estaMovDer() const;
	bool estaMovIzq() const;
	
	void seleccionar();
	void deseleccionar();
	
	void disparar();
	void disparo360();
	
	bool revisarColision(CL_CollisionOutline *outline, GameObject *otro);
	void setParametros(TanqueTipo tipo);
	void setNombre(CL_String nombre);
	void setEstaDisparando(bool estado);
	CL_String getNombre();
	TanqueTipo getTipo();
	
	virtual void draw();
	virtual bool update(float dt);

	CL_CollisionOutline getCuerpoCollision();
	CL_String nombre;

private:

	CL_Sprite *spriteCuerpo;
	CL_Sprite *spriteTorreta;
	CL_Sprite *spriteCuerpoNormal;
	CL_Sprite *spriteCuerpoMoviendose;
	CL_Sprite *spriteTorretaNormal;
	CL_Sprite *spriteTorretaDisparando;
	CL_Sprite *spriteTorretaRecargando;
	CL_Sprite *spriteSeleccionado;
	CL_Sprite *spriteTorretatFlashDisparo;
	CL_Sprite *spriteBrilloRojo;

	CL_CollisionOutline *cuerpoColision;
	
	CL_SoundBuffer *sonidoTorreta;

	float anguloCuerpo;
	float anguloDestCuerpo;
	float anguloDeltaCuerpo;
	float anguloTorreta;
	float anguloDestTorreta;
	float anguloDeltaTorreta;

	CL_Vec2f Posicion;
	CL_Vec2f PosDest;
	CL_Vec2f PosDelta;

	float velocidadGiroCuerpo;
	float velocidadGiroTorreta;
	float velocidad;
	
	bool seleccionado;

	bool seMovAdelante;
	bool seMovAtras;
	bool seMovDer;
	bool seMovIzq;

	bool estaDisparando;

	int vida;            
    int maxvida;
    int armadura;       
    int maxarmadura;

	int demoramuerte;
	int horamuerte;
	bool estamuerto;

	TanqueTipo miTipo;

	int score;


	//IA
	bool esIA;
	CL_Vec2f TargetPosIA;
	CL_Vec2f TargetPosTorretaIA;

	int	tiempoagarroPowerup;
	bool tienePowerup;
	int tipoPowerup;
};

#endif
