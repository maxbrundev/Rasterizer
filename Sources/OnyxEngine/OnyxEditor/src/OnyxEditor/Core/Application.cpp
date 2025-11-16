#include "OnyxEditor/Core/Application.h"

#include <chrono>
#include <iostream>

#include "OnyxEditor/Context/SDLDisplay.h"

#include "OnyxEditor/Data/Color.h"
#include "OnyxEditor/Rendering/Features/OutlineRenderFeature.h"

#include "OnyxEditor/Rendering/Features/ShadowMappingRenderFeature.h"
#include "OnyxEditor/Rendering/Passes/OpaquePass.h"
#include "OnyxEditor/Rendering/Passes/SelectedPass.h"
#include "OnyxEditor/Rendering/Passes/ShadowMappingPass.h"

#include "OnyxEditor/SceneSystem/Scenes/FlyingScene.h"
#include "OnyxEditor/SceneSystem/Scenes/ShadowDemoScene.h"

#include "OnyxEditor/Tools/Time/Clock.h"

#include "OnyxEditor/UI/Panels/DebugPanel.h"
#include "OnyxEditor/UI/Panels/FrameDebuggerPanel.h"

OnyxEditor::Core::Application::Application() :
m_isRunning(true),
m_panelsManager(m_canvas)
{
	auto& windowSize = m_context.Window->GetSize();

	m_context.Renderer->InitializeFrameDebugger(*m_context.SDLDriver, windowSize.first, windowSize.second);

	InitializeUI();
}

OnyxEditor::Core::Application::~Application()
{
	m_context.SceneManager.UnloadCurrentScene();
}

void OnyxEditor::Core::Application::Initialize()
{
	// Shadow mapping and outline features have significant performance cost.
	// For higher FPS in a playground scenes, comment out these passes/features and use a standard shader.
	m_context.Renderer->AddPass<Rendering::Passes::ShadowMappingPass>("ShadowMappingPass", Rendering::Settings::PassOrder::Opaque);
	m_context.Renderer->AddPass<Rendering::Passes::SelectedPass>("SelectedPass", Rendering::Settings::PassOrder::Debug);
	m_context.Renderer->AddPass<Rendering::Passes::OpaquePass>("Opaque", Rendering::Settings::PassOrder::Opaque + 1);
	m_context.Renderer->AddFeature<Rendering::Features::ShadowMappingRenderFeature>();
	m_context.Renderer->AddFeature<Rendering::Features::OutlineRenderFeature>();

	m_context.SceneManager.LoadScene(new SceneSystem::Scenes::ShadowDemoScene());
}

void OnyxEditor::Core::Application::Run()
{
	Tools::Time::Clock clock;
	OnyxEditor::Data::Color backGround(131, 188, 243);
	auto normalizedBackGroundColor = backGround.GetNormalizedVec4();

	UI::Panels::FrameDebuggerPanel& frameDebuggerPanel = m_panelsManager.GetPanelAs<UI::Panels::FrameDebuggerPanel>("FrameDebugger");
	UI::Panels::DebugPanel& debugPanel = m_panelsManager.GetPanelAs<UI::Panels::DebugPanel>("DebugPanel");

	while (IsRunning())
	{
		SceneSystem::AScene* scene = m_context.SceneManager.GetCurrentScene();

		m_context.Device->PollEvents();

		m_context.Renderer->GetFrameDebugger()->CheckStartFrameCapture();

		m_context.Renderer->DisplayClear();

		bool hasCapturedSteps = m_context.Renderer->GetFrameDebugger()->HasCapturedSteps();

		if (!hasCapturedSteps)
			scene->Update(clock.GetDeltaTime());

		auto& windowSize = m_context.Window->GetSize();

		m_context.Display->Resize(windowSize.first, windowSize.second);

		FrameInfo frameDescriptor;
		frameDescriptor.Width = windowSize.first;
		frameDescriptor.Height = windowSize.second;
		frameDescriptor.Camera = scene->GetCamera();
		frameDescriptor.CameraPosition = scene->GetCameraPosition();
		frameDescriptor.CameraRotation = scene->GetCameraRotation();

		m_context.Renderer->BeginFrame(frameDescriptor);

		const auto& view = scene->GetCamera()->GetViewMatrix();
		const auto& projection = scene->GetCamera()->GetProjectionMatrix();

		m_context.Renderer->UpdateViewProjectionMatrices(view,projection,scene->GetCameraPosition());

		m_context.Renderer->DrawFrame(scene);
		m_context.Renderer->EndFrame();

		m_context.Renderer->GetFrameDebugger()->CheckEndFrameCapture();

		debugPanel.UpdateWidgets();
		frameDebuggerPanel.UpdateWidgets();

		if (!frameDebuggerPanel.IsOpened())
		{
			auto* frameDebugger = m_context.Renderer->GetFrameDebugger();

			if (frameDebugger && frameDebugger->HasCapturedSteps())
			{
				frameDebugger->Clear();
			}
		}

		m_context.UIManager->Render();

		AmberGL::SwapBuffers();

		m_context.Display->UpdateDisplayTexture(AmberGL::GetFrontBufferData(),AmberGL::GetFrontBufferRowSize());

		m_context.SDLDriver->RenderClear();

		m_context.SDLDriver->RenderCopy(m_context.Display->GetTexture());

		m_context.UIManager->PostRender();
		m_context.SDLDriver->RenderPresent();

		m_context.InputManager->ClearEvents();

		clock.Update();
	}
}

bool OnyxEditor::Core::Application::IsRunning() const
{
	return m_isRunning && m_context.Window->IsActive();
}

void OnyxEditor::Core::Application::InitializeUI()
{
	//DebugPanel and FrameDebuggerPanel are quite rushed atm, goal was to make it exist before make it perfect e.g: the UpdateWidgets method that could be a virtual protected APanel method.
	UI::Panels::PanelSettings debugPanelSettings;
	debugPanelSettings.Closable = true;

	UI::Panels::PanelSettings frameDebuggerPanelSettings;
	frameDebuggerPanelSettings.Closable = true;
	frameDebuggerPanelSettings.MenuBar = true;

	UI::Panels::DebugPanel& debugPanel = m_panelsManager.CreatePanel<UI::Panels::DebugPanel>("DebugPanel", true, debugPanelSettings);
	UI::Panels::FrameDebuggerPanel& frameDebuggerPanel = m_panelsManager.CreatePanel<UI::Panels::FrameDebuggerPanel>("FrameDebugger", false, frameDebuggerPanelSettings);

	debugPanel.Initialize(m_context);

	debugPanel.SetFrameDebuggerPanel(&frameDebuggerPanel);

	frameDebuggerPanel.Initialize(m_context.Renderer->GetFrameDebugger(), m_context.SDLDriver->GetRenderer());

	m_context.UIManager->SetCanvas(m_canvas);
}