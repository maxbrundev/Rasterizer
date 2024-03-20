#include "Core/Application.h"

#include <chrono>

#include "Tools/Time/Clock.h"

Core::Application::Application(const Settings::WindowSettings& p_windowSettings, const Settings::DriverSettings& p_driverSettings) :
	m_context(p_windowSettings, p_driverSettings), m_cameraController(m_camera, m_cameraPosition), m_cameraPosition(0.0f, 0.0f, 10.0f),
	m_isRunning(true)
{
}

void Core::Application::Initialize()
{
}

void Core::Application::Run()
{
	Tools::Time::Clock clock;

	Data::Color backGround(0, 0, 0);

	Resources::Mesh cubeMesh;
	m_context.objParser.LoadOBJ("Resources/Models/Cube.obj", &cubeMesh);

	while (IsRunning())
	{
		m_context.device->PollEvents();

		m_context.renderer->Clear();

		m_context.renderer->Clear(backGround);
		m_context.renderer->ClearDepth();

		m_cameraController.Update(clock.GetDeltaTime());

		m_camera.CalculateMatrices(m_context.window->GetSize().first, m_context.window->GetSize().second, m_cameraPosition);

		glm::mat4 model(1.0f);

		glm::mat4 MVP = m_camera.GetProjectionMatrix() * m_camera.GetViewMatrix() * model;

		std::vector<Geometry::Vertex> verticesToRender;

		m_context.renderer->Draw(cubeMesh, MVP, model);

		m_context.renderer->Render();

		m_context.inputManager->ClearEvents();

		clock.Update();
	}
}

bool Core::Application::IsRunning() const
{
	return m_isRunning && m_context.window->IsActive();
}
