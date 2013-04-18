//ClanLib
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/GFrameHandler.h>
#include <ClanLib/L_ParticleSystem.h>


//C++
#include <list>

#include "servidor.h"
#include "cliente.h"
#include "powerup.h"
#include "tanque.h"
#include "misil.h"

class CL_Surface;
class CL_InputEvent;
class GameObject;
class Powerup;
class Tanque;
class Servidor;
class Cliente;
class CL_SoundBuffer;

using namespace std;

class Mundo
{

public:

	Mundo(CL_DisplayWindow &display_window);
	~Mundo();

	CL_ResourceManager *recursos; //falta get recursos
	
	CL_GraphicContext get_gc() { return gc; }

	void iniciarNivel();
	void iniciarParticulas();
	void matarParticulas();
	
	int revisarColision(CL_CollisionOutline *outline, GameObject *otro);

	void teclatanquedown(int idtanque,int tecla);
	void teclatanqueup(int idtanque,int tecla);

	void agregarObjeto(GameObject *object);
	void agregarPowerup(PowerupTipo tipo,CL_Vec2f _pos);
	void quitarPowerup(int id);
	void otorgaPowerup(int id_tanque, int id_power);
	void quitarTodosPowerup();
	void quitarTodosExcJugador();
	void actualiza_saludyscore();
	void actualiza_posiciones();
	void actualiza_SubID(int id_tanque);
	int getSubID();
	int getActualSubID();
	void reseteaTanques();
	void setSubID(int contador);
	
	bool puedeActualizar();
	bool revisarPosicion(CL_Vec2f _pos, int id);
	
	CL_Vec2f agregarTanque(int id,TanqueTipo tipo, CL_String nombre, bool esjugador, bool _esIA);
	void agregarTanque(int id,TanqueTipo tipo, CL_String nombre, bool esjugador, bool _esIA,CL_Vec2f _pos,float _angulo,float _angulotorreta);
	void quitarTanque(int id);

	CL_Vec2f getPowerupPosMin(PowerupTipo tipo,Tanque *tanque);
	CL_Vec2f getPosMinTanque(Tanque *tanque);
	Tanque* getTanquePosMin(Tanque *tanque);
	Tanque* getTanqueJugador();
	GameObject* getObjeto(int id);

	bool estoySolo(Tanque *tanque);

	CL_Vec2f getPosicionSpawn();
	
	void AgregarCadenaChat(CL_String cadena);

	CL_Vec2i getReloj();
	void setReloj(int _minutos, int _segundos);
	void setReloj(int _minutos, int _segundos, bool es_ascendente);
	void chequeaReloj();
	void matarTanque(int id, int tiempofin);
	void matarMisil(int id_asesino);
	void quitarObjeto(int id);
	Tanque* getTanqueID(int idtanque);

	CL_String CreaBarrita(int tam);
	CL_String getNombreJugador();

	void gameloop();

	std::list<GameObject *> objetos;
	std::list<int> id_quitar;
	std::list<Tanque *> tanques;
	std::list<Powerup *> powerups;
	std::list<CL_Pointf> puntos_explosion;

	void efectoExplosion(CL_Pointf pto);

private:

	Tanque *TanqueJugador;

	Servidor *servidor;
	Cliente *cliente;
	CL_String TipoConex;
	int NumJugadores;
	int contSubID;

	bool EsCliente;
	bool EsServidor;
	bool errormysql;
	bool menuingame;
	bool menuinicial;
	bool menumulti;
	bool deathmatchIA;
	bool DM_Multi;
	bool noupdatedb;
	bool muestra_est;

	bool borrarchat;	
	bool act_segundos;
	bool quit;

	void draw();
	void update();

	float calcularTiempoTranscurrido();

	void onKeyDown(const CL_InputEvent &key, const CL_InputState &state);
	void onKeyUp(const CL_InputEvent &key, const CL_InputState &state);
	void onMouseDown(const CL_InputEvent &key, const CL_InputState &state);
	void onMouseMove(const CL_InputEvent &key, const CL_InputState &state);
	void on_window_close();

	CL_Slot slotMouseDown;
	CL_Slot slotMouseUp;
	CL_Slot slotMouseMove;
	CL_Slot slotKeyDown;
	CL_Slot slotKeyUp;
	CL_Slot slot_quit;

	CL_String IP_Conectar;
	CL_String IP_Conectar2;
	
	CL_InputDevice teclado;
	CL_InputDevice mouse;

	CL_Font_System ftextura22;
	CL_Font_System ftextura20;
	CL_Font_System ftextura10;

	CL_Texture fondo;
	CL_Sprite Cursor;
	CL_Sprite LogoMini;
	CL_Sprite Boton;
	CL_Sprite Boton_ok;
	CL_Sprite Boton_cancelar;
	CL_Sprite Boton_cambiar;
	CL_Sprite AyudaCambio;
	CL_Sprite AyudaComo;
	CL_Sprite AyudaDia;
	CL_Sprite AyudaNada;
	CL_Gradient gdte;

	CL_Rect window_rect;
	CL_DisplayWindow ventana;
	CL_GraphicContext gc;

	//GUI
	CL_String NombreJugador;
	bool cambiartextbox;


	//Chat
	bool puedescribir;
	CL_String cadenachat;
	vector <CL_String> VChat;
	int tcadenachat;
	int ind_menu;
	
	//Tiempo
	int temp;
	int segundos;
	int minutos;
	bool rlj_asc;
	int rlj_segundos;
	int rlj_minutos;

	CL_String cadenatiempo;

	float tiempopowerup;

	CL_SoundBuffer *sonidocomienzo;
	CL_SoundBuffer *musica;	
	CL_SoundBuffer *sonido_ataereo;

	vector <CL_Vec2f> SpawnPoints;

    //Motor de Partículas

	GFrameHandler frame_handler;
	L_Particle* particula_1;
	L_Particle* particula_2;
	L_ShootingEffect* disparo_efecto;
	L_ExplosionEffect* exp_efecto;
	L_EffectEmitter* exp_emisor;
	L_MotionController motion_ctrl;
};

