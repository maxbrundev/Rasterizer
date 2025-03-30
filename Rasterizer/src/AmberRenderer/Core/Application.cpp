#include "AmberRenderer/Core/Application.h"

#include <chrono>
#include <glm/ext/matrix_transform.hpp>

#include "AmberRenderer/Rendering/SoftwareRenderer/AmberGL.h"
#include "AmberRenderer/Rendering/SoftwareRenderer/Programs/QuadNDC.h"
#include "AmberRenderer/Rendering/SoftwareRenderer/Programs/ShadowMapping.h"
#include "AmberRenderer/Rendering/SoftwareRenderer/Programs/ShadowMappingDepth.h"

#include "AmberRenderer/Resources/Mesh.h"
#include "AmberRenderer/Resources/Model.h"
#include "AmberRenderer/Resources/Loaders/ModelLoader.h"
#include "AmberRenderer/Resources/Loaders/ShaderLoader.h"
#include "AmberRenderer/Tools/Time/Clock.h"

//TODO: Scene system.
AmberRenderer::Core::Application::Application() :
m_cameraPosition({0.0f, 0.0f, 10.0f}),
m_cameraRotation(glm::quat({0.0f, 135.0f, 0.0f})),
m_cameraController(m_camera, m_cameraPosition, m_cameraRotation),
m_isRunning(true)
{
	AmberGL::Enable(GLR_DEPTH_TEST);
	AmberGL::Enable(GLR_DEPTH_WRITE);
	AmberGL::Enable(GLR_CULL_FACE);

	AmberGL::CullFace(GLR_BACK);
	m_shadowDepthShaderResource = Resources::Loaders::ShaderLoader::Create<Rendering::SoftwareRenderer::Programs::ShadowMappingDepth>("ShadowMapDepth");
}

AmberRenderer::Core::Application::~Application()
{
	Resources::Loaders::ModelLoader::Destroy(m_currentModel);
	Resources::Loaders::ShaderLoader::Destroy(m_shadowDepthShaderResource);

	AmberGL::Terminate();
}

void AmberRenderer::Core::Application::Initialize()
{
#ifndef RENDER_TEST
	m_context.Device->DropFileEvent.AddListener([this](const std::string& p_filePath)
	{
		if (m_currentModel != nullptr)
		{
			Resources::Loaders::ModelLoader::Destroy(m_currentModel);
		}

		m_currentModel = Resources::Loaders::ModelLoader::Create(p_filePath);

		for (Resources::Material* material : m_currentModel->GetMaterials())
		{
			material->SetShader(m_shadowDepthShaderResource);
		}
	});
#endif

	//TODO: ModelLoader instantiate related textures with flipVertically parameter at true, this result to flipped texture for this obj.
	m_currentModel = Resources::Loaders::ModelLoader::Create("Resources/Models/cube.obj");

	for (Resources::Material* material : m_currentModel->GetMaterials())
	{
		material->SetShader(m_shadowDepthShaderResource);
	}
}

void AmberRenderer::Core::Application::Run()
{
	Tools::Time::Clock clock;

	Data::Color backGround(70, 70, 70);

	uint32_t planeVAO;
	uint32_t planeVBO;

	std::vector<Geometry::Vertex> planeVertices = {
		{{-5.0f, -0.5f, 5.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{5.0f, -0.5f, 5.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{-5.0f, -0.5f, -5.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},

		{{5.0f, -0.5f, 5.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{5.0f, -0.5f, -5.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{-5.0f, -0.5f, -5.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}
	};

	AmberGL::GenBuffers(1, &planeVBO);
	AmberGL::BindBuffer(GLR_ARRAY_BUFFER, planeVBO);
	AmberGL::BufferData(GLR_ARRAY_BUFFER, planeVertices.size() * sizeof(Geometry::Vertex), planeVertices.data());

	AmberGL::GenVertexArrays(1, &planeVAO);
	AmberGL::BindVertexArray(planeVAO);

	AmberGL::BindBuffer(GLR_ARRAY_BUFFER, planeVBO);

	AmberGL::BindVertexArray(0);

	std::vector<uint32_t> indices = {};

	Resources::Mesh FLOORMesh(planeVertices, indices, 0);

	std::vector<Geometry::Vertex> vertices = {
		Geometry::Vertex(glm::vec3(-1.0f, -0.5f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f)),
		Geometry::Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f)),
		Geometry::Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f)),
		Geometry::Vertex(glm::vec3(-0.5f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f))
	};

	Resources::Mesh planeMesh(vertices, indices, 0);

	uint32_t VAO;
	uint32_t VBO;

	AmberGL::GenBuffers(1, &VBO);
	AmberGL::BindBuffer(GLR_ARRAY_BUFFER, VBO);
	AmberGL::BufferData(GLR_ARRAY_BUFFER, vertices.size() * sizeof(Geometry::Vertex), vertices.data());

	AmberGL::GenVertexArrays(1, &VAO);
	AmberGL::BindVertexArray(VAO);
	AmberGL::BindBuffer(GLR_ARRAY_BUFFER, VBO);
	AmberGL::BindVertexArray(0);

	Resources::Shader* quadShaderResource = Resources::Loaders::ShaderLoader::Create<Rendering::SoftwareRenderer::Programs::QuadNDC>("QuadNDC");
	Resources::Shader* shadowMappingResource = Resources::Loaders::ShaderLoader::Create<Rendering::SoftwareRenderer::Programs::ShadowMapping>("ShadowMapping");

	//TODO: Clean Texture generation.
	uint32_t m_shadowFBO;
	unsigned int depthMap;
	AmberGL::GenTextures(1, &depthMap);
	AmberGL::BindTexture(GLR_TEXTURE_2D, depthMap);
	AmberGL::TexImage2D(GLR_TEXTURE_2D, 0, GLR_DEPTH_COMPONENT, 1024, 1024, 0, GLR_DEPTH_COMPONENT, GLR_FLOAT, nullptr);
	AmberGL::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_MIN_FILTER, GLR_NEAREST);
	AmberGL::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_MAG_FILTER, GLR_NEAREST);
	AmberGL::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_WRAP_S, GLR_REPEAT);
	AmberGL::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_WRAP_T, GLR_REPEAT);
	AmberGL::BindTexture(GLR_TEXTURE_2D, 0);
	AmberGL::BindTexture(GLR_TEXTURE_2D, depthMap);
	AmberGL::BindTexture(GLR_TEXTURE_2D, 0);

	m_camera.SetProjectionMode(Rendering::Settings::EProjectionMode::PERSPECTIVE);

	AmberGL::BindTexture(GLR_TEXTURE_2D, depthMap);

	AmberGL::GenFramebuffers(1, &m_shadowFBO);
	AmberGL::BindFramebuffer(GLR_FRAMEBUFFER, m_shadowFBO);
	AmberGL::FramebufferTexture2D(GLR_FRAMEBUFFER,
	                              GLR_DEPTH_ATTACHMENT,
	                              GLR_TEXTURE_2D,
	                              depthMap,
	                              0);
	AmberGL::DrawBuffer(GL_NONE);
	AmberGL::ReadBuffer(GL_NONE);
	AmberGL::BindFramebuffer(GLR_FRAMEBUFFER, 0);

	quadShaderResource->SetUniform("u_DepthMap", 0);
	AmberGL::BindTexture(GLR_TEXTURE_2D, 0);

	Resources::Material shadowMapDepthMaterial;
	shadowMapDepthMaterial.SetShader(m_shadowDepthShaderResource);

	Resources::Material shadowMapMaterial;
	shadowMapMaterial.SetShader(shadowMappingResource);

	shadowMappingResource->SetUniform("u_DepthMap", 1);
	float accumulatedTime = 0.0f;

	float radius = 2.0f;

	glm::vec3 lightPos(5.0f, 6.0f, 5.0f);

	float nearPlane = 1.0f;
	float farPlane = 30.0f;
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));
	glm::mat4 lightProjection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, nearPlane, farPlane);
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	//GLRasterizer::WindowHint(GLR_SAMPLES, 8);
	//GLRasterizer::Enable(GLR_MULTISAMPLE);

	while (IsRunning())
	{
		m_context.Device->PollEvents();

		m_context.Renderer->RenderClear();

		m_context.Renderer->SetClearColor(backGround.GetNormalizedVec4());
		m_context.Renderer->Clear(true, true);

		for (Resources::Material* material : m_currentModel->GetMaterials())
		{
			material->SetShader(m_shadowDepthShaderResource);
		}

		accumulatedTime += clock.GetDeltaTime();

		lightPos.x = radius * cos(accumulatedTime);
		lightPos.z = radius * sin(accumulatedTime);

		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		lightSpaceMatrix = lightProjection * lightView;

		m_shadowDepthShaderResource->SetUniform("u_lightSpaceMatrix", lightSpaceMatrix);

		AmberGL::BindFramebuffer(GLR_FRAMEBUFFER, m_shadowFBO);
		m_context.Renderer->SetViewport(0, 0, 1024, 1024);
		m_context.Renderer->Clear(false, true);

		glm::mat4 cubeModel(1.0f);
		cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 3.0f, 2.0f));
		m_shadowDepthShaderResource->SetUniform("u_Model", cubeModel);
		m_context.Renderer->Draw(*m_currentModel, &shadowMapDepthMaterial);

		glm::mat4 cube2Model(1.0f);
		cube2Model = glm::translate(cube2Model, glm::vec3(0.0f, 1.0f, 2.0f));
		cube2Model = glm::scale(cube2Model, glm::vec3(2.0f, 0.2f, 2.0f));

		m_shadowDepthShaderResource->SetUniform("u_Model", cube2Model);
		m_context.Renderer->Draw(*m_currentModel, &shadowMapDepthMaterial);

		glm::mat4 planeModel(1.0f);
		planeModel = glm::translate(planeModel, glm::vec3(0.0f, 0.0f, 0.0f));
		planeModel = glm::scale(planeModel, glm::vec3(1.0f, 1.0f, 1.0f));
		m_shadowDepthShaderResource->SetUniform("u_Model", planeModel);
		m_shadowDepthShaderResource->Bind();
		AmberGL::BindVertexArray(planeVAO);
		AmberGL::DrawArrays(GLR_TRIANGLES, 0, FLOORMesh.GetVertexCount());
		AmberGL::BindVertexArray(0);

		AmberGL::BindFramebuffer(GLR_FRAMEBUFFER, 0);

		AmberGL::Viewport(0, 0, 800, 600);
		m_context.Renderer->Clear(true, true);

		m_cameraController.Update(clock.GetDeltaTime());
		m_camera.ComputeMatrices(m_context.Window->GetSize().first, m_context.Window->GetSize().second,
		                         m_cameraPosition, m_cameraRotation);
		const auto& view = m_camera.GetViewMatrix();
		const auto& projection = m_camera.GetProjectionMatrix();

		for (Resources::Material* material : m_currentModel->GetMaterials())
		{
			material->SetShader(shadowMappingResource);
		}

		shadowMappingResource->SetUniform("u_View", view);
		shadowMappingResource->SetUniform("u_Projection", projection);
		shadowMappingResource->SetUniform("u_lightPos", lightPos);
		shadowMappingResource->SetUniform("u_lightSpaceMatrix", lightSpaceMatrix);
		shadowMappingResource->SetUniform("u_ViewPos", m_cameraPosition);

		shadowMappingResource->SetUniform("u_Model", cubeModel);
		AmberGL::ActiveTexture(GLR_TEXTURE0 + 1);
		AmberGL::BindTexture(GLR_TEXTURE_2D, depthMap);
		m_context.Renderer->Draw(*m_currentModel, &shadowMapMaterial);
		AmberGL::BindTexture(GLR_TEXTURE_2D, 0);

		shadowMappingResource->SetUniform("u_Model", cube2Model);
		AmberGL::ActiveTexture(GLR_TEXTURE0 + 1);
		AmberGL::BindTexture(GLR_TEXTURE_2D, depthMap);
		m_context.Renderer->Draw(*m_currentModel, &shadowMapMaterial);
		AmberGL::BindTexture(GLR_TEXTURE_2D, 0);

		shadowMappingResource->SetUniform("u_Model", planeModel);
		shadowMappingResource->Bind();
		AmberGL::BindVertexArray(planeVAO);
		AmberGL::ActiveTexture(GLR_TEXTURE0 + 1);
		AmberGL::BindTexture(GLR_TEXTURE_2D, depthMap);
		AmberGL::DrawArrays(GLR_TRIANGLES, 0, FLOORMesh.GetVertexCount());
		AmberGL::BindTexture(GLR_TEXTURE_2D, 0);
		AmberGL::BindVertexArray(0);

		//Debug depth texture.
		quadShaderResource->Bind();
		AmberGL::BindVertexArray(VAO);
		AmberGL::ActiveTexture(GLR_TEXTURE0);
		AmberGL::BindTexture(GLR_TEXTURE_2D, depthMap);
		AmberGL::DrawArrays(GLR_TRIANGLE_STRIP, 0, planeMesh.GetVertexCount());
		AmberGL::BindVertexArray(0);
		quadShaderResource->Unbind();

		m_context.Renderer->Render();
		m_context.InputManager->ClearEvents();

		clock.Update();
	}

	Resources::Loaders::ShaderLoader::Destroy(quadShaderResource);
	Resources::Loaders::ShaderLoader::Destroy(shadowMappingResource);
}

bool AmberRenderer::Core::Application::IsRunning() const
{
	return m_isRunning && m_context.Window->IsActive();
}
