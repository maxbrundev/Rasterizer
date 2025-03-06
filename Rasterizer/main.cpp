#define SDL_MAIN_HANDLED

#include "AmberRenderer/Core/Application.h"

int main()
{
	AmberRenderer::Core::Application application;
	application.Initialize();
	application.Run();

	return 0;
}
