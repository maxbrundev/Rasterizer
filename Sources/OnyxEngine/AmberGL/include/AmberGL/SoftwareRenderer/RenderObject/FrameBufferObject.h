#pragma once

#include "AmberGL/SoftwareRenderer/RenderObject/FrameBufferObjectData.h"
#include "AmberGL/SoftwareRenderer/RenderObject/TextureObject.h"

namespace AmberGL::SoftwareRenderer::RenderObject
{
	struct FrameBufferObject
	{
		FrameBufferObjectData<RGBA8>* ColorBuffer = nullptr;
		FrameBufferObjectData<Depth>* DepthBuffer = nullptr;
		FrameBufferObjectData<Stencil>* StencilBuffer = nullptr;

		TextureObject* AttachedTexture = nullptr;
		uint32_t ID = 0;
		uint16_t Attachment;
		//bool ColorWriteEnabled = true;

		~FrameBufferObject()
		{
			delete ColorBuffer;
			delete DepthBuffer;
			delete StencilBuffer;
		}
	};
}
