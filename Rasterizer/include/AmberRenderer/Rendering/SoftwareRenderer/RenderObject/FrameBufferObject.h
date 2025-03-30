#pragma once

#include "AmberRenderer/Rendering/SoftwareRenderer/RenderObject/FrameBufferObjectData.h"
#include "AmberRenderer/Rendering/SoftwareRenderer/RenderObject/TextureObject.h"

namespace AmberRenderer::Rendering::SoftwareRenderer::RenderObject
{
	struct FrameBufferObject
	{
		FrameBufferObjectData<RGBA8>* ColorBuffer = nullptr;
		FrameBufferObjectData<Depth>* DepthBuffer = nullptr;
		TextureObject* AttachedTexture = nullptr;
		uint32_t ID = 0;
		bool ColorWriteEnabled = true;


		~FrameBufferObject()
		{
			delete ColorBuffer;
			delete DepthBuffer;
		}
	};
}
