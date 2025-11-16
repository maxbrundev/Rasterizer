#include <SDL2/SDL.h>

#include "OnyxEditor/Core/Application.h"

int SDL_main(int argc, char* argv[])
{
	OnyxEditor::Core::Application application;
	application.Initialize();
	application.Run();

	return 0;
}
