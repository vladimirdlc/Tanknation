//JATG

//Clanlib
#include <ClanLib/display.h>
#include <ClanLib/sound.h>
#include <ClanLib/vorbis.h>
#include <ClanLib/gl1.h>
#include <ClanLib/network.h>
#include <ClanLib/core.h>
#include <ClanLib/application.h>

//C/C++
#include <string.h>

//#include "servidor.h
#include "app.h"
#include "mundo.h"

#define DEBUG    


// Clase llamada por CL_ClanApplication
class Program
{
public:
	static int main(const std::vector<CL_String> &args)
	{
		// Inicializa los componentes base de ClanLib
		CL_SetupCore setup_core;

		// Inicializa los componentes de pantalla
		CL_SetupDisplay setup_display;

		// Inicializa los drivers de OpenGL
		CL_SetupGL1 setup_gl;
		
		// Configuracion de sonido
		CL_SetupSound setup_sound;

		// Configuracion de Vorbis Ogg
		CL_SetupVorbis setup_vorbis;

		CL_SetupNetwork setup_network;

		// Comienza la aplicacion
		Application app;
		int retval = app.main(args);
		return retval;
	}
};

// Instancia CL_ClanApplication, informadon donde el Programa está localizado
CL_ClanApplication app(&Program::main);

int Application::main(const std::vector<CL_String> &args)
{	

	try
	{

		#ifdef DEBUG
		// Crea una ventan en consola para la salida de texto si no está disponible
		CL_ConsoleWindow console("JATG", 80, 1000); // 1000 permite una barra vertical
		CL_ConsoleLogger logger;
		#endif


		// Crea la ventana
		CL_DisplayWindowDescription desc;
		desc.set_title("JATG");

		desc.set_size(CL_Size(1024,768), true);	// Usa esta resolución

		desc.set_fullscreen(true);
		desc.set_decorations(false);

		CL_DisplayWindow window(desc);

		CL_GraphicContext gc = window.get_gc();
		CL_Mat4f matrix = CL_Mat4f::scale( (float) gc.get_width() / 1024.0f, (float) gc.get_height() / 768.0f, 1.0f);
		gc.set_modelview(matrix);

		

		CL_SoundOutput output(44100); //Inicializa Frecuencia

		// Crea el mundo
		Mundo mundo(window);

		// Corre el bucle del juego
		mundo.gameloop();

			
		
	}
	catch (CL_Exception& exception)
	{
		CL_Console::write_line("Excepcion Tomada:");
		CL_Console::write_line(exception.message);

		std::vector<CL_String> stacktrace = exception.get_stack_trace();
		int tamaño = stacktrace.size();
		if (tamaño > 0)
		{
			CL_Console::write_line("Stack Trace:");
			for (int i=0; i < tamaño; i++)
			{
				CL_Console::write_line(stacktrace[i]);
			}
		}

		CL_Console::wait_for_key(); //Espera por una tecla
	}

	return 0; 
}
