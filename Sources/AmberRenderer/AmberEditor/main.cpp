#define SDL_MAIN_HANDLED

#include "AmberEditor/Core/Application.h"

int main()
{
	AmberEditor::Core::Application application;
	application.Initialize();
	application.Run();

	return 0;
}
