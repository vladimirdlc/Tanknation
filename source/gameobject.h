#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

class Mundo;
class CL_CollisionOutline;

class GameObject
{

public:

	GameObject(Mundo *mundo) { this->mundo = mundo; }

	virtual void draw() {};
	virtual bool update(float dt) { return false; }
	virtual bool revisarColision(CL_CollisionOutline *outline, GameObject *object) { return false; }

	int getID() { return id; }
	void setID(int id) { this->id = id; }
	
protected:

	Mundo *mundo;
	int id;

};

#endif
