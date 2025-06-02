#pragma once

#include <cstdint>

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

#include "AmberEditor/Buffers/FrameBuffer.h"

#include "AmberEditor/Entities/Camera.h"

struct FrameInfo
{
	uint16_t Width = 0;
	uint16_t Height = 0;
	glm::vec3 CameraPosition;
	glm::quat CameraRotation;
	AmberEditor::Buffers::FrameBuffer* OutputBuffer = nullptr;
	AmberEditor::Entities::Camera* Camera = nullptr;
};
