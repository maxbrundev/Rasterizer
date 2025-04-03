#include <SDL2/SDL.h>

#include "AmberEditor/Core/Application.h"

int SDL_main(int argc, char* argv[])
{
	AmberEditor::Core::Application application;
	application.Initialize();
	application.Run();

	return 0;
}
