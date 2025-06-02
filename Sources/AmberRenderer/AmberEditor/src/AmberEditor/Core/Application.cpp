#include "AmberEditor/Core/Application.h"

#include <chrono>

#include "AmberEditor/Data/Color.h"

#include "AmberEditor/Rendering/Features/ShadowMappingFeature.h"
#include "AmberEditor/Rendering/Passes/OpaquePass.h"
#include "AmberEditor/Rendering/Passes/ShadowMappingPass.h"

#include "AmberEditor/Resources/Shaders/ShadowMapping.h"
#include "AmberEditor/Resources/Shaders/ShadowMappingDepth.h"

#include "AmberEditor/SceneSystem/Scenes/ShadowDemoScene.h"

#include "AmberEditor/Tools/Time/Clock.h"

AmberEditor::Core::Application::Application() :
m_cameraController(m_camera, m_cameraPosition, m_cameraRotation),
m_cameraPosition({ 0.0f, 0.0f, 10.0f }),
m_cameraRotation(glm::quat({ 0.0f, 135.0f, 0.0f })),
m_isRunning(true)
{
}

AmberEditor::Core::Application::~Application()
{
	m_context.sceneManager.UnloadCurrentScene();
}

void AmberEditor::Core::Application::Initialize()
{
	m_context.ShaderManager.GetShader<Resources::Shaders::ShadowMappingDepth>("ShadowMapDepth");
	m_context.ShaderManager.GetShader<Resources::Shaders::ShadowMapping>("ShadowMap");

	m_context.Renderer->AddFeature<Rendering::Features::ShadowMappingFeature>();
	m_context.Renderer->AddPass<Rendering::Passes::ShadowMappingPass>("Shadows", Rendering::Settings::PassOrder::Shadows);
	m_context.Renderer->AddPass<Rendering::Passes::OpaquePass>("Opaque", Rendering::Settings::PassOrder::Opaque);

	m_camera.SetProjectionMode(Rendering::Settings::EProjectionMode::PERSPECTIVE);

	m_context.sceneManager.LoadScene(new SceneSystem::Scenes::ShadowDemoScene());
}

void AmberEditor::Core::Application::Run()
{
	Tools::Time::Clock clock;
	AmberEditor::Data::Color backGround(70, 70, 70);

	while (IsRunning())
	{
		// Handle events
		m_context.Device->PollEvents();

		// Clear display
		m_context.Renderer->DisplayClear();
		auto test = backGround.GetNormalizedVec4();
		m_context.Renderer->SetClearColor(test.x, test.y, test.z, test.w);

		// Update camera
		m_cameraController.Update(clock.GetDeltaTime());
		m_camera.ComputeMatrices(m_context.Window->GetSize().first, m_context.Window->GetSize().second, m_cameraPosition, m_cameraRotation);
		const auto& view = m_camera.GetViewMatrix();
		const auto& projection = m_camera.GetProjectionMatrix();

		// Update scene
		auto* currentScene = m_context.sceneManager.GetCurrentScene();
		currentScene->Update(clock.GetDeltaTime());

		FrameInfo frameDescriptor;
		frameDescriptor.Width = 800;
		frameDescriptor.Height = 600;
		frameDescriptor.Camera = &m_camera;
		frameDescriptor.CameraPosition = m_cameraController.GetPosition();
		frameDescriptor.CameraRotation = m_cameraController.GetRotation();


		auto& models = currentScene->GetModels();

		for (auto* model : models)
		{
			for (auto* material : model->GetMaterials())
			{
				material->SetUniform("u_View", view);
				material->SetUniform("u_Projection", projection);
			}
		}
		

		// Render scene with pipeline
		m_context.Renderer->BeginFrame(frameDescriptor);
		m_context.Renderer->DrawFrame(currentScene);
		m_context.Renderer->EndFrame();

		m_context.Renderer->DisplayPresent();

		m_context.InputManager->ClearEvents();

		clock.Update();
	}
}

bool AmberEditor::Core::Application::IsRunning() const
{
	return m_isRunning && m_context.Window->IsActive();
}