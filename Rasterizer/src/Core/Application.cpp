#include "Core/Application.h"

#include <chrono>
#include <glm/ext/matrix_transform.hpp>

#include "Buffers/VertexBuffer.h"
#include "Buffers/IndexBuffer.h"

#include "Rendering/DefaultShader.h"
#include "Rendering/GLRasterizer.h"

#include "Resources/Model.h"
#include "Resources/Mesh.h"
#include "Resources/Loaders/ModelLoader.h"

#include "Tools/Time/Clock.h"

//#define RENDER_TEST

Core::Application::Application(const Context::Settings::WindowSettings& p_windowSettings, const Context::Settings::DriverSettings& p_driverSettings) :
m_context(p_windowSettings, p_driverSettings),
m_cameraController(m_camera, m_cameraPosition),
m_cameraPosition(0.0f, 0.0f, 10.0f),
m_isRunning(true)
{
	GLRasterizer::Enable(GLR_DEPTH_TEST);
	GLRasterizer::Enable(GLR_DEPTH_WRITE);
	GLRasterizer::Enable(GLR_CULL_FACE);

	GLRasterizer::CullFace(GLR_BACK);
}

Core::Application::~Application()
{
	Resources::Loaders::ModelLoader::Destroy(m_currentModel);

	GLRasterizer::Terminate();
}

void Core::Application::Initialize()
{
#ifndef RENDER_TEST
	m_context.device->DropFileEvent.AddListener([this](const std::string& p_filePath) 
	{
		if (m_currentModel != nullptr)
		{
			Resources::Loaders::ModelLoader::Destroy(m_currentModel);
		}

		m_currentModel = Resources::Loaders::ModelLoader::Create(p_filePath);

		for (Resources::Material* material : m_currentModel->GetMaterials())
		{
			material->SetShader(&basicShader);
		}
	});
#endif

	m_defaultMaterial.SetShader(&basicShader);

	//TODO: ModelLoader instantiate related textures with flipVertically parameter at true, this result to flipped texture for this obj.
	m_currentModel = Resources::Loaders::ModelLoader::Create("Resources/Models/DamagedHelmet/wavefront/DamagedHelmet.obj");

	for (Resources::Material* material : m_currentModel->GetMaterials())
	{
		material->SetShader(&basicShader);
	}
}

void Core::Application::Run()
{
	Tools::Time::Clock clock;

	Data::Color backGround(70, 70, 70);

	Buffers::VertexBuffer vertices;
	vertices.Vertices = {
		{{-1.0, 0.0, -1.0}},
		{{-1.0, 0.0, 1.0}},
		{{1.0, 0.0, 1.0}},
		{{1.0, 0.0, -1.0}}
	};

	Buffers::IndexBuffer indices;
	indices.Indices = { 0, 1, 2, 2, 3, 0 };

	Resources::Mesh planeMesh(vertices.Vertices, indices.Indices, 0);

	while (IsRunning())
	{
		m_context.device->PollEvents();

		// Testing MSAA
		if (m_context.inputManager->IsKeyPressed(Inputs::EKey::KEY_2))
		{
			m_context.renderer->SetSamples(2);
		}

		if (m_context.inputManager->IsKeyPressed(Inputs::EKey::KEY_4))
		{
			m_context.renderer->SetSamples(4);
		}

		if (m_context.inputManager->IsKeyPressed(Inputs::EKey::KEY_8))
		{
			m_context.renderer->SetSamples(8);
		}
		
		m_context.renderer->Clear();

		m_context.renderer->Clear(backGround);
		m_context.renderer->ClearDepth();

		m_cameraController.Update(clock.GetDeltaTime());

		m_camera.ComputeMatrices(m_context.window->GetSize().first, m_context.window->GetSize().second, m_cameraPosition);

		glm::mat4 model(1.0f);

		//TODO: For Sponza model. Will fix.
		//model = glm::scale(glm::mat4(1.0f), glm::vec3(0.03f));
		const auto& view = m_camera.GetViewMatrix();
		const auto& projection = m_camera.GetProjectionMatrix();

		basicShader.SetUniformMat4("u_Model", model);
		basicShader.SetUniformMat4("u_View", view);
		basicShader.SetUniformMat4("u_Projection", projection);
		basicShader.SetUniformVec3("u_ViewPos", m_cameraPosition);

#ifdef RENDER_TEST
		//RENDER TEST
		m_context.renderer->Draw(*m_currentModel, &m_defaultMaterial);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
		basicShader.SetUniformMat4("u_Model", model);
		//m_context.renderDriver->SetCapability(Rendering::Settings::DEPTH_TEST, false);
		//m_context.renderDriver->SetDepthWriting(false);
		m_context.renderDriver->SetRasterizationMode(Rendering::Settings::ERasterizationMode::LINE);
		m_context.renderer->Draw(*m_currentModel, &m_defaultMaterial);
		m_context.renderDriver->SetRasterizationMode(Rendering::Settings::ERasterizationMode::FILL);
		//m_context.renderDriver->SetCapability(Rendering::Settings::DEPTH_TEST, true);
		//m_context.renderDriver->SetDepthWriting(true);

		m_context.renderDriver->SetRasterizationMode(Rendering::Settings::ERasterizationMode::POINT);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.0f, 0.0f));
		basicShader.SetUniformMat4("u_Model", model);
		m_context.renderer->Draw(*m_currentModel, &m_defaultMaterial);
		m_context.renderDriver->SetRasterizationMode(Rendering::Settings::ERasterizationMode::FILL);
#else
		m_context.renderer->Draw(*m_currentModel, &m_defaultMaterial);
#endif

		m_context.renderer->Render();
		m_context.inputManager->ClearEvents();

		clock.Update();
	}
}

bool Core::Application::IsRunning() const
{
	return m_isRunning && m_context.window->IsActive();
}