#include "Core/Application.h"

#include <chrono>
#include <glm/ext/matrix_transform.hpp>

#include "Resources/Model.h"
#include "Tools/Time/Clock.h"
#include "Buffers/VertexBuffer.h"
#include "Buffers/IndexBuffer.h"
#include "Rendering/DefaultShader.h"
#include "Resources/Mesh.h"
#include "Resources/Loaders/ModelLoader.h"
#include "Resources/Loaders/TextureLoader.h"

Rendering::Application::Application(const Settings::WindowSettings& p_windowSettings, const Settings::DriverSettings& p_driverSettings) :
	m_context(p_windowSettings, p_driverSettings), m_cameraController(m_camera, m_cameraPosition), m_cameraPosition(0.0f, 0.0f, 10.0f),
	m_isRunning(true)
{
}

void Rendering::Application::Initialize()
{
}

void Rendering::Application::Run()
{
	Tools::Time::Clock clock;

	Data::Color backGround(0, 0, 0);

	Resources::Model* modelCube = Resources::Loaders::ModelLoader::Create("Resources/Models/Terriermon.obj");
	Resources::Model* modelCube2 = Resources::Loaders::ModelLoader::Create("Resources/Models/WarGreymon.obj");
	Resources::Model* modelCube3 = Resources::Loaders::ModelLoader::Create("Resources/Models/Gabumon.obj");

	DefaultShader basicShader;

	Buffers::VertexBuffer vertices;
	vertices.Vertices = {
		{{-1.0, 0.0, -1.0}},
		{{-1.0, 0.0, 1.0}},
		{{1.0, 0.0, 1.0}},
		{{1.0, 0.0, -1.0}}
	};

	Buffers::IndexBuffer indices;
	indices.Indices = { 0, 1, 2, 2, 3, 0 };

	Resources::Mesh planeMesh(vertices.Vertices, indices.Indices);

	auto texture = Resources::Loaders::TextureLoader::Create("Resources/Textures/Terriermon.png", true, Resources::ETextureFilteringMode::NEAREST, ETextureWrapMode::CLAMP, true);
	auto texture2 = Resources::Loaders::TextureLoader::Create("Resources/Textures/WarGreymon.png", true, Resources::ETextureFilteringMode::NEAREST, ETextureWrapMode::CLAMP, true);
	auto texture3 = Resources::Loaders::TextureLoader::Create("Resources/Textures/Gabumon.png", true, Resources::ETextureFilteringMode::LINEAR, ETextureWrapMode::REPEAT, true);
	int samples = 0;

	while (IsRunning())
	{
		m_context.device->PollEvents();

		// Testing MSAA
		if (m_context.inputManager->IsKeyPressed(Inputs::EKey::KEY_2))
		{
			samples = 2;

			m_context.renderer->SetSamples(samples);
		}

		if (m_context.inputManager->IsKeyPressed(Inputs::EKey::KEY_4))
		{
			samples = 4;

			m_context.renderer->SetSamples(samples);
		}

		if (m_context.inputManager->IsKeyPressed(Inputs::EKey::KEY_8))
		{
			samples = 8;

			m_context.renderer->SetSamples(samples);
		}
		
		m_context.renderer->Clear();

		m_context.renderer->Clear(backGround);
		m_context.renderer->ClearDepth();

		m_cameraController.Update(clock.GetDeltaTime());

		m_camera.ComputeMatrices(m_context.window->GetSize().first, m_context.window->GetSize().second, m_cameraPosition);

		glm::mat4 model(1.0f);

		const auto& view = m_camera.GetViewMatrix();
		const auto& projection = m_camera.GetProjectionMatrix();

		basicShader.SetUniform("u_Model", model);
		basicShader.SetUniform("u_View", view);
		basicShader.SetUniform("u_Projection", projection);
		basicShader.SetUniform("u_ViewPos", m_cameraPosition);

		basicShader.SetSample("u_DiffuseMap", texture);
		m_context.renderer->Draw(TRIANGLE, *modelCube, basicShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
		basicShader.SetUniform("u_Model", model);
		basicShader.SetSample("u_DiffuseMap", texture2);
		m_context.renderer->Draw(TRIANGLE, *modelCube2, basicShader);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
		basicShader.SetUniform("u_Model", model);
		basicShader.SetSample("u_DiffuseMap", texture3);

		m_context.renderer->Draw(LINE, *modelCube3, basicShader);

		m_context.renderer->SetDepthTest(false);
		//m_context.renderer->DrawMesh(planeMesh, basicShader);
		m_context.renderer->SetDepthTest(true);
		m_context.renderer->DrawLine({ 0.0f, 0.0f, 0.0f }, { 5.0f, 0.0f, 0.0f }, basicShader, Data::Color::Red);

		m_context.renderer->Render();
		m_context.inputManager->ClearEvents();

		clock.Update();
	}

	Resources::Loaders::ModelLoader::Destroy(modelCube);
}

bool Rendering::Application::IsRunning() const
{
	return m_isRunning && m_context.window->IsActive();
}