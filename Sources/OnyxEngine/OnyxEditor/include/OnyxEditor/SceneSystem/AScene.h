#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "OnyxEditor/Core/CameraController.h"

#include "OnyxEditor/Entities/Camera.h"

#include "OnyxEditor/Resources/Model.h"

namespace OnyxEditor::SceneSystem
{
	class AScene
	{
	public:
		AScene(const std::string& p_name);
		virtual ~AScene();

		virtual void Load() = 0;
		virtual void Unload() = 0;
		virtual void Update(float p_deltaTime) = 0;

		Entities::Camera* GetCamera() { return &m_camera; }
		const glm::vec3& GetCameraPosition() const { return m_cameraPosition; }
		const glm::quat& GetCameraRotation() const { return m_cameraRotation; }

		std::vector<Resources::Model*>& GetModels();

	public:
		const std::string Name;

	protected:
		std::vector<Resources::Model*> m_models;

		Entities::Camera m_camera;
		Core::CameraController m_cameraController;
		glm::vec3 m_cameraPosition;
		glm::quat m_cameraRotation;
	};
}