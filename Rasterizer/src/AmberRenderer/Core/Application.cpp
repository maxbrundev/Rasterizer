#include "AmberRenderer/Core/Application.h"

#include <chrono>
#include <glm/ext/matrix_transform.hpp>

#include "AmberRenderer/Rendering/Rasterizer/GLRasterizer.h"
#include "AmberRenderer/Rendering/Rasterizer/Shaders/QuadNDC.h"
#include "AmberRenderer/Rendering/Rasterizer/Shaders/ShadowMapping.h"
#include "AmberRenderer/Rendering/Rasterizer/Shaders/ShadowMappingDepth.h"

#include "AmberRenderer/Resources/Model.h"
#include "AmberRenderer/Resources/Mesh.h"
#include "AmberRenderer/Resources/Loaders/ModelLoader.h"
#include "AmberRenderer/Resources/Loaders/TextureLoader.h"

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
	m_shadowBuffer = new Rendering::Rasterizer::Buffers::FrameBuffer<RGBA8>(1024, 1024);
	m_shadowDepthBuffer = new Rendering::Rasterizer::Buffers::FrameBuffer<float>(1024, 1024);

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

	unsigned char* emptyDepthData = new unsigned char[1024 * 1024 * 4];
	std::memset(emptyDepthData, 0, 1024 * 1024 * 4);

	//TODO: Clean Texture generation.
	m_shadowMapTexture = new Resources::Texture("", 1024, 1024, 32, emptyDepthData, Resources::Settings::ETextureFilteringMode::NEAREST, Resources::Settings::ETextureWrapMode::CLAMP, false);

	delete[] emptyDepthData;

	m_shadowDepthBuffer->BindTexture(m_shadowMapTexture);

	m_camera.SetProjectionMode(Rendering::Settings::EProjectionMode::PERSPECTIVE);

	Rendering::Rasterizer::Shaders::QuadNDC QuadNDCShader;
	QuadNDCShader.SetSample("u_DepthMap", m_shadowMapTexture);

	auto woodTexture = Resources::Loaders::TextureLoader::CreateColor
	(
		(255 << 24) | (255 << 16) | (255 << 8) | 255,
		Resources::Settings::ETextureFilteringMode::NEAREST,
		Resources::Settings::ETextureWrapMode::CLAMP
	);

	Resources::Material shadowMapDepthMaterial;
	shadowMapDepthMaterial.SetShader(&m_shadowMapDepthShader);

	Rendering::Rasterizer::Shaders::ShadowMapping shadowMapping;
	Resources::Material shadowMapMaterial;
	shadowMapMaterial.SetShader(&shadowMapping);

	float accumulatedTime = 0.0f;

	float radius = 2.0f;

	glm::vec3 lightPos(5.0f, 6.0f, 5.0f);

	float nearPlane = 1.0f;
	float farPlane = 30.0f;
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));
	glm::mat4 lightProjection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, nearPlane, farPlane);
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	//TODO: Fix.
	//GLRasterizer::WindowHint(GLR_SAMPLES, 4);
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

		m_shadowMapDepthShader.SetUniformMat4("u_lightSpaceMatrix", lightSpaceMatrix);

		m_context.Renderer->SetViewport(0, 0, 1024, 1024);
		GLRasterizer::SetActiveBuffers(m_shadowBuffer, m_shadowDepthBuffer);

		m_context.Renderer->Clear(false, true);

		glm::mat4 cubeModel(1.0f);
		cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 3.0f, 2.0f));
		
		m_shadowMapDepthShader.SetUniformMat4("u_Model", cubeModel);
		m_context.Renderer->Draw(*m_currentModel, &shadowMapDepthMaterial);

		glm::mat4 planeModel(1.0f);
		planeModel = glm::translate(planeModel, glm::vec3(0.0f, 0.0f, 0.0f));
		planeModel = glm::scale(planeModel, glm::vec3(1.0f, 1.0f, 1.0f));
		m_shadowMapDepthShader.SetUniformMat4("u_Model", planeModel);
		
		GLRasterizer::UseProgram(&m_shadowMapDepthShader);
		GLRasterizer::BindVertexArray(planeVAO);
		GLRasterizer::DrawArrays(GLR_TRIANGLES, 0, FLOORMesh.GetVertexCount());
		GLRasterizer::BindVertexArray(0);

		m_shadowDepthBuffer->Bind();
		GLRasterizer::SetActiveBuffers(GLRasterizer::GetFrameBuffer(), GLRasterizer::GetDepthBuffer());
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

		shadowMapping.SetUniformMat4("u_View", view);
		shadowMapping.SetUniformMat4("u_Projection", projection);
		shadowMapping.SetUniformVec3("u_lightPos", lightPos);
		shadowMapping.SetUniformMat4("u_lightSpaceMatrix", lightSpaceMatrix);
		shadowMapping.SetSample("u_DepthMap", m_shadowMapTexture);

		shadowMapping.SetUniformMat4("u_Model", cubeModel);
		m_context.Renderer->Draw(*m_currentModel, &shadowMapMaterial);
		
		shadowMapping.SetUniformMat4("u_Model", planeModel);
		shadowMapping.SetSample("u_DiffuseMap", woodTexture);
		GLRasterizer::UseProgram(&shadowMapping);
		GLRasterizer::BindVertexArray(planeVAO);
		GLRasterizer::DrawArrays(GLR_TRIANGLES, 0, FLOORMesh.GetVertexCount());
		GLRasterizer::BindVertexArray(0);

		//Debug depth texture.
		GLRasterizer::UseProgram(&QuadNDCShader);
		GLRasterizer::BindVertexArray(VAO);
		GLRasterizer::DrawArrays(GLR_TRIANGLE_STRIP, 0,  planeMesh.GetVertexCount());
		GLRasterizer::BindVertexArray(0);

		m_context.Renderer->Render();
		m_context.InputManager->ClearEvents();

		clock.Update();
	}

	delete m_shadowMapTexture;
	m_shadowMapTexture = nullptr;
}

bool AmberRenderer::Core::Application::IsRunning() const
{
	return m_isRunning && m_context.Window->IsActive();
}