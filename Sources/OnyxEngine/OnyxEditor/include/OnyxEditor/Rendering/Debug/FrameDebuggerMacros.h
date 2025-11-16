#pragma once

#include <string>

namespace OnyxEditor::Rendering
{
	class ARenderer;
	class Renderer;
}
namespace OnyxEditor::Rendering::Debug
{
	class FrameDebugger;
}

#define FRAME_DEBUGGER_GET(renderer) (renderer).GetFrameDebugger()

#define FRAME_DEBUGGER_EXECUTE(renderer, code) \
	if (auto* debugger = FRAME_DEBUGGER_GET(renderer)) \
	{ \
		if (debugger->IsCapturingFrame()) \
		{ \
			code \
		} \
	}

#define FRAME_DEBUG_RECORD_CLEAR(renderer, colorBuf, depthBuf, stencilBuf) \
	FRAME_DEBUGGER_EXECUTE(renderer, { \
		std::string description = "Clear "; \
		if (colorBuf) description += "Color "; \
		if (depthBuf) description += "Depth "; \
		if (stencilBuf) description += "Stencil "; \
		uint32_t fbWidth = AmberGL::GetActiveFrameBufferWidth(); \
		uint32_t fbHeight = AmberGL::GetActiveFrameBufferHeight(); \
		uint32_t textureID = (renderer).CaptureFrameToTexture(fbWidth, fbHeight); \
		debugger->RecordClear(description, textureID); \
	})

#define FRAME_DEBUG_RECORD_DRAW_CALL(renderer, desc, triangles, vertices, material, pass) \
	FRAME_DEBUGGER_EXECUTE(renderer, { \
		uint32_t fbWidth = AmberGL::GetActiveFrameBufferWidth(); \
		uint32_t fbHeight = AmberGL::GetActiveFrameBufferHeight(); \
		uint32_t textureID = (renderer).CaptureFrameToTexture(fbWidth, fbHeight); \
		debugger->RecordDrawCall(desc, triangles, vertices, material, pass, textureID); \
	})

#define FRAME_DEBUG_RECORD_SET_RENDER_TARGET(renderer, desc) \
	FRAME_DEBUGGER_EXECUTE(renderer, { \
		uint32_t fbWidth = AmberGL::GetActiveFrameBufferWidth(); \
		uint32_t fbHeight = AmberGL::GetActiveFrameBufferHeight(); \
		uint32_t textureID = (renderer).CaptureFrameToTexture(fbWidth, fbHeight); \
		debugger->RecordSetRenderTarget(desc, textureID); \
	})

#define FRAME_DEBUG_RECORD_DRAW_CALL_AUTO(renderer, drawable, passName) \
	FRAME_DEBUGGER_EXECUTE(renderer, { \
		std::string description = "Draw: "; \
		std::string materialName = "Unknown Material"; \
		if ((drawable).Material && (drawable).Material->GetShader()) \
		{ \
			materialName = (drawable).Material->GetName(); \
			description += materialName; \
		} \
		else \
		{ \
			description += "Unknown Material"; \
		} \
		uint32_t triangles = (drawable).Mesh->GetIndexCount() > 0 ? \
		(drawable).Mesh->GetIndexCount() / 3 : (drawable).Mesh->GetVertexCount() / 3; \
		uint32_t vertices = (drawable).Mesh->GetVertexCount(); \
		uint32_t fbWidth = AmberGL::GetActiveFrameBufferWidth(); \
		uint32_t fbHeight = AmberGL::GetActiveFrameBufferHeight(); \
		uint32_t textureID = (renderer).CaptureFrameToTexture(fbWidth, fbHeight); \
		debugger->RecordDrawCall(description, triangles, vertices, materialName, passName, textureID); \
	})