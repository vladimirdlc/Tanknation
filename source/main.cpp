
//JATG


//Clanlib
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/network.h>
#include <ClanLib/gl1.h>
#include <ClanLib/application.h>

//C/C++
#include <string.h>
#include <ctype.h>

#include "framerate_counter.h"
#include "server.h"

//Constantes


//Variables Globales
bool quit = false;
CL_String cadenachat = "";




int x=200,y=200;

class JATG
{
	public:
		static int main(const std::vector<CL_String> &args);
};

CL_ClanApplication app(&JATG::main);


void onWindowClose()
{
	quit=true;
}

void onKeyDown(const CL_InputEvent &event, const CL_InputState &state)
{

	//if((event.id >= CL_KEY_0)&&(event.id <= CL_KEY_Z))
	//{
	//	cadenachat.push_back(tolower(event.id));
	//}
	//else if(event.id==CL_KEY_BACKSPACE) 
	//{
	//	if(!cadenachat.empty())
	//	{
	//		cadenachat.erase(cadenachat.size()-1);
	//	}
	//}
	//else if(event.id==CL_KEY_SPACE)
	//{
	//	cadenachat +=" ";
	//}

	/*switch (event.id)
	{	
		case CL_KEY_LEFT: 
				x-=5; break;
		case CL_KEY_RIGHT: 
				x+=5; break;
		case CL_KEY_UP: 
				y-=5; break;
		case CL_KEY_DOWN: 
				y+=5; break;	
	}*/

}

//Main
int JATG::main(const std::vector<CL_String> &args)
{

	CL_SetupCore setup_core;
	CL_SetupDisplay setup_display;
	CL_SetupGL1 setup_gl;
	CL_SetupNetwork setup_network;

	CL_ConsoleWindow console("Consola Debug", 160, 1000);
	CL_ConsoleLogger logger;

	Server server;
	CL_Thread thread;
	thread.start(&server);

	CL_DisplayWindow window("JATG", 640, 468);
	CL_GraphicContext gc = window.get_gc();
	CL_InputDevice keyboard = window.get_ic().get_keyboard();
	CL_InputDevice mouse = window.get_ic().get_mouse();
	CL_Slot skey = keyboard.sig_key_up().connect(&onKeyDown);
	CL_Slot squit = window.sig_window_close().connect(&onWindowClose);


	CL_Sprite sprite(gc,"data\\logo.tga");
	CL_CollisionOutline outline("data\\logo.tga");
	outline.set_translation(300,265);
	CL_Sprite sprite2(gc,"data\\medi.png");
	CL_CollisionOutline outline2("data\\medi.png");
	
	//CL_Sprite Cursor(gc,"data\\Cursor.png");
	

	CL_Font_Texture ftextura(gc, L"Tahoma", 20);

	FramerateCounter frameratecounter;
	



	//GameLoop
	while (!quit)
	{
		/*CL_Pointf pmouse((float)mouse.get_x(),(float)mouse.get_y());

		if(outline.collide(outline2))
		{
			quit = true;
		}

		outline2.set_translation(x,y);*/

		gc.clear(CL_Colorf::black);
		ftextura.draw_text(gc, 100, 100,server.getcadena(), CL_Colorf::lightseagreen);
		ftextura.draw_text(gc,10,50,cl_format("Cursor: X=%1, Y=%2",mouse.get_x(),mouse.get_y()),CL_Colorf::gold);
		CL_String fps = cl_format("%1 fps", frameratecounter.get_framerate());
		ftextura.draw_text(gc,10,30, fps);
		sprite.draw(gc,300,265);
		sprite2.draw(gc,x,y);

		
		
		window.flip(0);
		frameratecounter.frame_shown();
	
		
		CL_DisplayMessageQueue::process();
	
		CL_System::sleep(10);

	}

	thread.join();
	return 0;
}

