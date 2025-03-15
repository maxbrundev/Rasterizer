#include "AmberRenderer/Core/Application.h"

#include <chrono>
#include <glm/ext/matrix_transform.hpp>

#include "AmberRenderer/Rendering/Rasterizer/GLRasterizer.h"
#include "AmberRenderer/Rendering/Rasterizer/Shaders/QuadNDC.h"
#include "AmberRenderer/Rendering/Rasterizer/Shaders/ShadowMapping.h"
#include "AmberRenderer/Rendering/Rasterizer/Shaders/ShadowMappingDepth.h"

#include "AmberRenderer/Resources/Mesh.h"
#include "AmberRenderer/Resources/Model.h"
#include "AmberRenderer/Resources/Loaders/ModelLoader.h"

#include "AmberRenderer/Tools/Time/Clock.h"

AmberRenderer::Core::Application::Application() :
m_cameraPosition({ 0.0f, 0.0f, 10.0f }),
m_cameraRotation (glm::quat({ 0.0f, 135.0f, 0.0f })),
m_cameraController(m_camera, m_cameraPosition, m_cameraRotation),
m_isRunning(true)
{
	GLRasterizer::Enable(GLR_DEPTH_TEST);
	GLRasterizer::Enable(GLR_DEPTH_WRITE);
	GLRasterizer::Enable(GLR_CULL_FACE);

	GLRasterizer::CullFace(GLR_BACK);
}

AmberRenderer::Core::Application::~Application()
{
	Resources::Loaders::ModelLoader::Destroy(m_currentModel);

	GLRasterizer::Terminate();
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
			material->SetShader(&m_shadowMapDepthShader);
		}
	});
#endif

	//TODO: ModelLoader instantiate related textures with flipVertically parameter at true, this result to flipped texture for this obj.
	m_currentModel = Resources::Loaders::ModelLoader::Create("Resources/Models/cube.obj");

	for (Resources::Material* material : m_currentModel->GetMaterials())
	{
		material->SetShader(&m_shadowMapDepthShader);
	}
}

void AmberRenderer::Core::Application::Run()
{
	Tools::Time::Clock clock;

	Data::Color backGround(70, 70, 70);

	uint32_t planeVAO;
	uint32_t planeVBO;

	std::vector<Geometry::Vertex> planeVertices = {
		{{-5.0f, -0.5f,  5.0f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
	{{ 5.0f, -0.5f,  5.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
	{{-5.0f, -0.5f, -5.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},

	{{ 5.0f, -0.5f,  5.0f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
	{{ 5.0f, -0.5f, -5.0f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
	{{-5.0f, -0.5f, -5.0f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}
	};

	GLRasterizer::GenBuffers(1, &planeVBO);
	GLRasterizer::BindBuffer(GLR_ARRAY_BUFFER, planeVBO);
	GLRasterizer::BufferData(GLR_ARRAY_BUFFER,
		planeVertices.size() * sizeof(Geometry::Vertex),
		planeVertices.data());

	GLRasterizer::GenVertexArrays(1, &planeVAO);
	GLRasterizer::BindVertexArray(planeVAO);

	GLRasterizer::BindBuffer(GLR_ARRAY_BUFFER, planeVBO);

	GLRasterizer::BindVertexArray(0);

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

	GLRasterizer::GenBuffers(1, &VBO);
	GLRasterizer::BindBuffer(GLR_ARRAY_BUFFER, VBO);
	GLRasterizer::BufferData(GLR_ARRAY_BUFFER,
		vertices.size() * sizeof(Geometry::Vertex),
		vertices.data());

	GLRasterizer::GenVertexArrays(1, &VAO);
	GLRasterizer::BindVertexArray(VAO);
	GLRasterizer::BindBuffer(GLR_ARRAY_BUFFER, VBO);
	GLRasterizer::BindVertexArray(0);

	//TODO: Clean Texture generation.
	uint32_t m_shadowFBO;
	unsigned int depthMap;
	GLRasterizer::GenTextures(1, &depthMap);
	GLRasterizer::BindTexture(GLR_TEXTURE_2D, depthMap);
	GLRasterizer::TexImage2D(GLR_TEXTURE_2D, 0, GLR_DEPTH_COMPONENT, 1024, 1024, 0, GLR_DEPTH_COMPONENT, GLR_FLOAT, nullptr);
	GLRasterizer::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_MIN_FILTER, GLR_NEAREST);
	GLRasterizer::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_MAG_FILTER, GLR_NEAREST);
	GLRasterizer::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_WRAP_S, GLR_REPEAT);
	GLRasterizer::TexParameteri(GLR_TEXTURE_2D, GLR_TEXTURE_WRAP_T, GLR_REPEAT);
	GLRasterizer::BindTexture(GLR_TEXTURE_2D, 0);
	GLRasterizer::BindTexture(GLR_TEXTURE_2D, depthMap);
	GLRasterizer::BindTexture(GLR_TEXTURE_2D, 0);

	m_camera.SetProjectionMode(Rendering::Settings::EProjectionMode::PERSPECTIVE);

	Rendering::Rasterizer::Shaders::QuadNDC QuadNDCShader;
	GLRasterizer::BindTexture(GLR_TEXTURE_2D, depthMap);

	GLRasterizer::GenFramebuffers(1, &m_shadowFBO);
	GLRasterizer::BindFramebuffer(GLR_FRAMEBUFFER, m_shadowFBO);
	GLRasterizer::FramebufferTexture2D(GLR_FRAMEBUFFER,
		GLR_DEPTH_ATTACHMENT,
		GLR_TEXTURE_2D,
		depthMap,
		0);
	GLRasterizer::DrawBuffer(GL_NONE);
	GLRasterizer::ReadBuffer(GL_NONE);
	GLRasterizer::BindFramebuffer(GLR_FRAMEBUFFER, 0);

	QuadNDCShader.SetUniform("u_DepthMap", 0);
	GLRasterizer::BindTexture(GLR_TEXTURE_2D, 0);

	Resources::Material shadowMapDepthMaterial;
	shadowMapDepthMaterial.SetShader(&m_shadowMapDepthShader);

	Rendering::Rasterizer::Shaders::ShadowMapping shadowMapping;
	Resources::Material shadowMapMaterial;
	shadowMapMaterial.SetShader(&shadowMapping);
	shadowMapping.SetUniform("u_DepthMap", 1);
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
			material->SetShader(&m_shadowMapDepthShader);
		}

		accumulatedTime += clock.GetDeltaTime();

		lightPos.x = radius * cos(accumulatedTime);
		lightPos.z = radius * sin(accumulatedTime);

		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		lightSpaceMatrix = lightProjection * lightView;

		m_shadowMapDepthShader.SetUniform("u_lightSpaceMatrix", lightSpaceMatrix);

		GLRasterizer::BindFramebuffer(GLR_FRAMEBUFFER, m_shadowFBO);
		m_context.Renderer->SetViewport(0, 0, 1024, 1024);
		m_context.Renderer->Clear(false, true);

		glm::mat4 cubeModel(1.0f);
		cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 3.0f, 2.0f));
		m_shadowMapDepthShader.SetUniform("u_Model", cubeModel);
		m_context.Renderer->Draw(*m_currentModel, &shadowMapDepthMaterial);

		glm::mat4 cube2Model(1.0f);
		cube2Model = glm::translate(cube2Model, glm::vec3(0.0f, 1.0f, 2.0f));
		cube2Model = glm::scale(cube2Model, glm::vec3(2.0f, 0.2f, 2.0f));

		m_shadowMapDepthShader.SetUniform("u_Model", cube2Model);
		m_context.Renderer->Draw(*m_currentModel, &shadowMapDepthMaterial);

		glm::mat4 planeModel(1.0f);
		planeModel = glm::translate(planeModel, glm::vec3(0.0f, 0.0f, 0.0f));
		planeModel = glm::scale(planeModel, glm::vec3(1.0f, 1.0f, 1.0f));
		m_shadowMapDepthShader.SetUniform("u_Model", planeModel);
		GLRasterizer::UseProgram(&m_shadowMapDepthShader);
		GLRasterizer::BindVertexArray(planeVAO);
		GLRasterizer::DrawArrays(GLR_TRIANGLES, 0, FLOORMesh.GetVertexCount());
		GLRasterizer::BindVertexArray(0);

		GLRasterizer::BindFramebuffer(GLR_FRAMEBUFFER, 0);
		
		GLRasterizer::Viewport(0, 0, 800, 600);
		m_context.Renderer->Clear(true, true);

		m_cameraController.Update(clock.GetDeltaTime());
		m_camera.ComputeMatrices(m_context.Window->GetSize().first, m_context.Window->GetSize().second, m_cameraPosition, m_cameraRotation);
		const auto& view = m_camera.GetViewMatrix();
		const auto& projection = m_camera.GetProjectionMatrix();

		for (Resources::Material* material : m_currentModel->GetMaterials())
		{
			material->SetShader(&shadowMapping);
		}

		shadowMapping.SetUniform("u_View", view);
		shadowMapping.SetUniform("u_Projection", projection);
		shadowMapping.SetUniform("u_lightPos", lightPos);
		shadowMapping.SetUniform("u_lightSpaceMatrix", lightSpaceMatrix);
		shadowMapping.SetUniform("u_ViewPos", m_cameraPosition);

		shadowMapping.SetUniform("u_Model", cubeModel);
		GLRasterizer::ActiveTexture(GLR_TEXTURE0 + 1);
		GLRasterizer::BindTexture(GLR_TEXTURE_2D, depthMap);
		m_context.Renderer->Draw(*m_currentModel, &shadowMapMaterial);
		GLRasterizer::BindTexture(GLR_TEXTURE_2D, 0);

		shadowMapping.SetUniform("u_Model", cube2Model);
		GLRasterizer::ActiveTexture(GLR_TEXTURE0 + 1);
		GLRasterizer::BindTexture(GLR_TEXTURE_2D, depthMap);
		m_context.Renderer->Draw(*m_currentModel, &shadowMapMaterial);
		GLRasterizer::BindTexture(GLR_TEXTURE_2D, 0);

		shadowMapping.SetUniform("u_Model", planeModel);
		GLRasterizer::UseProgram(&shadowMapping);
		GLRasterizer::BindVertexArray(planeVAO);
		GLRasterizer::ActiveTexture(GLR_TEXTURE0 + 1);
		GLRasterizer::BindTexture(GLR_TEXTURE_2D, depthMap);
		GLRasterizer::DrawArrays(GLR_TRIANGLES, 0, FLOORMesh.GetVertexCount());
		GLRasterizer::BindTexture(GLR_TEXTURE_2D, 0);
		GLRasterizer::BindVertexArray(0);

		//Debug depth texture.
		GLRasterizer::UseProgram(&QuadNDCShader);
		GLRasterizer::BindVertexArray(VAO);
		GLRasterizer::ActiveTexture(GLR_TEXTURE0);
		GLRasterizer::BindTexture(GLR_TEXTURE_2D, depthMap);
		GLRasterizer::DrawArrays(GLR_TRIANGLE_STRIP, 0,  planeMesh.GetVertexCount());
		GLRasterizer::BindVertexArray(0);

		m_context.Renderer->Render();
		m_context.InputManager->ClearEvents();

		clock.Update();
	}
}

bool AmberRenderer::Core::Application::IsRunning() const
{
	return m_isRunning && m_context.Window->IsActive();
}