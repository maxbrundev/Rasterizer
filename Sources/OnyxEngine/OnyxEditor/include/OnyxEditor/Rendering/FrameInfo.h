#pragma once

#include <cstdint>

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

#include "OnyxEditor/Buffers/FrameBuffer.h"

#include "OnyxEditor/Entities/Camera.h"

struct FrameInfo
{
	uint16_t Width = 0;
	uint16_t Height = 0;
	glm::vec3 CameraPosition;
	glm::quat CameraRotation;
	OnyxEditor::Buffers::FrameBuffer* OutputBuffer = nullptr;
	OnyxEditor::Entities::Camera* Camera = nullptr;
};
