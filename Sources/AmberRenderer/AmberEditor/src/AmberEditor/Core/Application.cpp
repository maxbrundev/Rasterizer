#include "AmberEditor/Core/Application.h"

#include <chrono>
#include <glm/ext/matrix_transform.hpp>

#include <AmberGL/SoftwareRenderer/AmberGL.h>

#include "AmberEditor/Data/Color.h"

#include "AmberEditor/Resources/Mesh.h"
#include "AmberEditor/Resources/Model.h"
#include "AmberEditor/Resources/Loaders/ModelLoader.h"
#include "AmberEditor/Resources/Loaders/ShaderLoader.h"
#include "AmberEditor/Resources/Loaders/TextureLoader.h"
#include "AmberEditor/Resources/Shaders/QuadNDC.h"
#include "AmberEditor/Resources/Shaders/ShadowMapping.h"
#include "AmberEditor/Resources/Shaders/ShadowMappingDepth.h"

#include "AmberEditor/Tools/Time/Clock.h"
#include "AmberGL/SoftwareRenderer/Defines.h"

//TODO: Scene system.
AmberEditor::Core::Application::Application() :
m_cameraPosition({0.0f, 0.0f, 10.0f}),
m_cameraRotation(glm::quat({0.0f, 135.0f, 0.0f})),
m_cameraController(m_camera, m_cameraPosition, m_cameraRotation),
m_isRunning(true)
{
	m_context.ShaderManager.GetShader<Resources::Shaders::ShadowMappingDepth>("ShadowMapDepth");
	m_shadowDepthShaderResource = m_context.ShaderManager.GetResource("ShadowMapDepth");
}

AmberEditor::Core::Application::~Application()
{
	Resources::Loaders::ModelLoader::Destroy(m_currentModel);
	Resources::Loaders::ShaderLoader::Destroy(m_shadowDepthShaderResource);

}

void AmberEditor::Core::Application::Initialize()
{
#ifndef RENDER_TEST
	m_context.Device->DropFileEvent.AddListener([this](const std::string& p_filePath)
	{
		if (m_currentModel != nullptr)
		{
			Resources::Loaders::ModelLoader::Destroy(m_currentModel);
		}

		m_currentModel = Resources::Loaders::ModelLoader::Create(p_filePath);
		m_currentModel->FillWithMaterial(m_shadowMapMaterial);
		//Todo: quick hack to simply set the model textures, need clean.
		Resources::Loaders::ModelLoader::GenerateModelMaterials(*m_currentModel);
	});
#endif

	//TODO: ModelLoader instantiate related textures with flipVertically parameter at true, this result to flipped texture for this obj.
	m_currentModel = Resources::Loaders::ModelLoader::Create("Resources/Editor/Models/Cube.obj");
}

void AmberEditor::Core::Application::Run()
{
	Tools::Time::Clock clock;

	AmberEditor::Data::Color backGround(70, 70, 70);

	uint32_t planeVAO;
	uint32_t planeVBO;

	std::vector<AmberGL::Geometry::Vertex> planeVertices = {
		{{-5.0f, -0.5f, 5.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{5.0f, -0.5f, 5.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{-5.0f, -0.5f, -5.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},

		{{5.0f, -0.5f, 5.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{5.0f, -0.5f, -5.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{-5.0f, -0.5f, -5.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}
	};

	AmberGL::GenBuffers(1, &planeVBO);
	AmberGL::BindBuffer(AGL_ARRAY_BUFFER, planeVBO);
	AmberGL::BufferData(AGL_ARRAY_BUFFER, planeVertices.size() * sizeof(AmberGL::Geometry::Vertex), planeVertices.data());

	AmberGL::GenVertexArrays(1, &planeVAO);
	AmberGL::BindVertexArray(planeVAO);

	AmberGL::BindBuffer(AGL_ARRAY_BUFFER, planeVBO);

	AmberGL::BindVertexArray(0);

	std::vector<uint32_t> indices = {};

	Resources::Mesh FLOORMesh(planeVertices, indices, 0);

	std::vector<AmberGL::Geometry::Vertex> vertices = {
		AmberGL::Geometry::Vertex(glm::vec3(-1.0f, -0.5f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f)),
		AmberGL::Geometry::Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f)),
		AmberGL::Geometry::Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f)),
		AmberGL::Geometry::Vertex(glm::vec3(-0.5f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f))
	};

	Resources::Mesh planeMesh(vertices, indices, 0);

	uint32_t VAO;
	uint32_t VBO;

	AmberGL::GenBuffers(1, &VBO);
	AmberGL::BindBuffer(AGL_ARRAY_BUFFER, VBO);
	AmberGL::BufferData(AGL_ARRAY_BUFFER, vertices.size() * sizeof(AmberGL::Geometry::Vertex), vertices.data());

	AmberGL::GenVertexArrays(1, &VAO);
	AmberGL::BindVertexArray(VAO);
	AmberGL::BindBuffer(AGL_ARRAY_BUFFER, VBO);
	AmberGL::BindVertexArray(0);

	auto quadShaderResource = Resources::Loaders::ShaderLoader::Create<Resources::Shaders::QuadNDC>();
	auto shadowMappingResource = Resources::Loaders::ShaderLoader::Create<Resources::Shaders::ShadowMapping>();

	//TODO: Clean Texture generation.
	uint32_t m_shadowFBO;
	unsigned int depthMap;
	AmberGL::GenTextures(1, &depthMap);
	AmberGL::BindTexture(AGL_TEXTURE_2D, depthMap);
	AmberGL::TexImage2D(AGL_TEXTURE_2D, 0, AGL_DEPTH_COMPONENT, 1024, 1024, 0, AGL_DEPTH_COMPONENT, AGL_FLOAT, nullptr);
	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MIN_FILTER, AGL_NEAREST);
	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MAG_FILTER, AGL_NEAREST);
	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_WRAP_S, AGL_REPEAT);
	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_WRAP_T, AGL_REPEAT);
	AmberGL::BindTexture(AGL_TEXTURE_2D, 0);
	AmberGL::BindTexture(AGL_TEXTURE_2D, depthMap);
	AmberGL::BindTexture(AGL_TEXTURE_2D, 0);

	m_camera.SetProjectionMode(Rendering::Settings::EProjectionMode::PERSPECTIVE);

	AmberGL::BindTexture(AGL_TEXTURE_2D, depthMap);

	AmberGL::GenFrameBuffers(1, &m_shadowFBO);
	AmberGL::BindFrameBuffer(AGL_FRAMEBUFFER, m_shadowFBO);
	AmberGL::FrameBufferTexture2D(AGL_FRAMEBUFFER,
	                              AGL_DEPTH_ATTACHMENT,
	                              AGL_TEXTURE_2D,
	                              depthMap,
	                              0);

	AmberGL::BindFrameBuffer(AGL_FRAMEBUFFER, 0);

	quadShaderResource->SetUniform("u_DepthMap", 0);
	AmberGL::BindTexture(AGL_TEXTURE_2D, 0);

	m_shadowMapDepthMaterial.SetShader(m_shadowDepthShaderResource);

	m_shadowMapMaterial.SetShader(shadowMappingResource);

	shadowMappingResource->SetUniform("u_DepthMap", 1);

	m_currentModel->FillWithMaterial(m_shadowMapMaterial);
	Resources::Loaders::ModelLoader::GenerateModelMaterials(*m_currentModel);

	float accumulatedTime = 0.0f;

	float radius = 2.0f;

	glm::vec3 lightPos(5.0f, 6.0f, 5.0f);

	float nearPlane = 1.0f;
	float farPlane = 30.0f;
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));
	glm::mat4 lightProjection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, nearPlane, farPlane);
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	//AmberGL::WindowHint(AGL_SAMPLES, 8);
	//AmberGL::Enable(AGL_MULTISAMPLE);

	while (IsRunning())
	{
		m_context.Device->PollEvents();

		m_context.Renderer->DisplayClear();

		m_context.Renderer->SetClearColor(backGround.GetNormalizedVec4());
		m_context.Renderer->Clear(true, true);

		m_currentModel->FillWithMaterial(m_shadowMapDepthMaterial);

		accumulatedTime += clock.GetDeltaTime();

		lightPos.x = radius * cos(accumulatedTime);
		lightPos.z = radius * sin(accumulatedTime);

		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		lightSpaceMatrix = lightProjection * lightView;

		m_shadowMapDepthMaterial.SetUniform("u_lightSpaceMatrix", lightSpaceMatrix);

		AmberGL::BindFrameBuffer(AGL_FRAMEBUFFER, m_shadowFBO);
		m_context.Renderer->SetViewport(0, 0, 1024, 1024);
		m_context.Renderer->Clear(false, true);

		glm::mat4 cubeModel(1.0f);
		cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 3.0f, 2.0f));
		m_shadowMapDepthMaterial.SetUniform("u_Model", cubeModel);
		m_context.Renderer->Draw(*m_currentModel, &m_shadowMapDepthMaterial);

		glm::mat4 cube2Model(1.0f);
		cube2Model = glm::translate(cube2Model, glm::vec3(0.0f, 1.0f, 2.0f));
		cube2Model = glm::scale(cube2Model, glm::vec3(2.0f, 0.2f, 2.0f));

		m_shadowMapDepthMaterial.SetUniform("u_Model", cube2Model);
		m_context.Renderer->Draw(*m_currentModel, &m_shadowMapDepthMaterial);

		glm::mat4 planeModel(1.0f);
		planeModel = glm::translate(planeModel, glm::vec3(0.0f, 0.0f, 0.0f));
		planeModel = glm::scale(planeModel, glm::vec3(1.0f, 1.0f, 1.0f));
		m_shadowDepthShaderResource->SetUniform("u_Model", planeModel);
		m_shadowDepthShaderResource->Bind();
		AmberGL::BindVertexArray(planeVAO);
		AmberGL::DrawArrays(AGL_TRIANGLES, 0, FLOORMesh.GetVertexCount());
		AmberGL::BindVertexArray(0);

		AmberGL::BindFrameBuffer(AGL_FRAMEBUFFER, 0);

		AmberGL::Viewport(0, 0, 800, 600);
		m_context.Renderer->Clear(true, true);

		m_cameraController.Update(clock.GetDeltaTime());
		m_camera.ComputeMatrices(m_context.Window->GetSize().first, m_context.Window->GetSize().second, m_cameraPosition, m_cameraRotation);
		const auto& view = m_camera.GetViewMatrix();
		const auto& projection = m_camera.GetProjectionMatrix();

		m_currentModel->FillWithMaterial(m_shadowMapMaterial);

		m_shadowMapMaterial.SetUniform("u_View", view);
		m_shadowMapMaterial.SetUniform("u_Projection", projection);
		m_shadowMapMaterial.SetUniform("u_lightPos", lightPos);
		m_shadowMapMaterial.SetUniform("u_lightSpaceMatrix", lightSpaceMatrix);
		m_shadowMapMaterial.SetUniform("u_ViewPos", m_cameraPosition);

		m_shadowMapMaterial.SetUniform("u_Model", cubeModel);

		AmberGL::ActiveTexture(AGL_TEXTURE0 + 1);
		AmberGL::BindTexture(AGL_TEXTURE_2D, depthMap);
		m_context.Renderer->Draw(*m_currentModel, &m_shadowMapMaterial);
		AmberGL::BindTexture(AGL_TEXTURE_2D, 0);

		m_shadowMapMaterial.SetUniform("u_Model", cube2Model);
		AmberGL::ActiveTexture(AGL_TEXTURE0 + 1);
		AmberGL::BindTexture(AGL_TEXTURE_2D, depthMap);
		m_context.Renderer->Draw(*m_currentModel, &m_shadowMapMaterial);
		AmberGL::BindTexture(AGL_TEXTURE_2D, 0);

		shadowMappingResource->SetUniform("u_Model", planeModel);
		shadowMappingResource->Bind();
		AmberGL::BindVertexArray(planeVAO);
		AmberGL::ActiveTexture(AGL_TEXTURE0 + 1);
		AmberGL::BindTexture(AGL_TEXTURE_2D, depthMap);
		AmberGL::DrawArrays(AGL_TRIANGLES, 0, FLOORMesh.GetVertexCount());
		AmberGL::BindTexture(AGL_TEXTURE_2D, 0);
		AmberGL::BindVertexArray(0);

		//Debug depth texture.
		AmberGL::BlitFrameBuffer(
			m_shadowFBO, 0,
			0, 0, 1024, 1024,
			0, 0, 100, 100,
			AGL_COLOR_BUFFER_BIT,
			AGL_LINEAR
		);

		m_context.Renderer->DisplayPresent();
		m_context.InputManager->ClearEvents();

		clock.Update();
	}

	Resources::Loaders::ShaderLoader::Destroy(quadShaderResource);
	Resources::Loaders::ShaderLoader::Destroy(shadowMappingResource);
}

bool AmberEditor::Core::Application::IsRunning() const
{
	return m_isRunning && m_context.Window->IsActive();
}
