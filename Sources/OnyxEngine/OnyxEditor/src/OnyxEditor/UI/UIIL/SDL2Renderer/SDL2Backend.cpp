#include "OnyxEditor/UI/UIIL/SDL2Renderer/SDL2Backend.h"

#include <SDL2/SDL.h>

#include "OnyxEditor/ImGUI/imgui_impl_sdl2.h"
#include "OnyxEditor/ImGUI/imgui_impl_sdlrenderer2.h"

template <>
bool OnyxEditor::UI::UIIL::SDL2Backend::Initialize(void* p_window, void* p_context)
{
	if (m_context.IsInitialized)
		return false;

	m_context.Window = p_window;
	m_context.Renderer = p_context;

	SDL_Window* sdlWindow = static_cast<SDL_Window*>(p_window);
	SDL_Renderer* sdlRenderer = static_cast<SDL_Renderer*>(p_context);

	bool success = ImGui_ImplSDL2_InitForSDLRenderer(sdlWindow, sdlRenderer);

	if (success)
	{
		success = ImGui_ImplSDLRenderer2_Init(sdlRenderer);
	}

	m_context.IsInitialized = success;

	return success;
}

template <>
void OnyxEditor::UI::UIIL::SDL2Backend::Shutdown()
{
	if (!m_context.IsInitialized)
		return;

	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	m_context.IsInitialized = false;
}

template <>
void OnyxEditor::UI::UIIL::SDL2Backend::NewFrame()
{
	if (m_context.IsInitialized)
	{
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
	}
}

template <>
void OnyxEditor::UI::UIIL::SDL2Backend::RenderDrawData()
{
	if (m_context.IsInitialized)
	{
		SDL_Renderer* renderer = static_cast<SDL_Renderer*>(m_context.Renderer);
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
	}
}

template <>
std::string OnyxEditor::UI::UIIL::SDL2Backend::GetBackendName() const
{
	return "SDL2";
}
