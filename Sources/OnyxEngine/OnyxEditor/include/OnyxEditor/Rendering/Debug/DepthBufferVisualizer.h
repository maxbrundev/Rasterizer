#pragma once

#include <cstdint>

namespace OnyxEditor::Rendering::Debug
{
	class DepthBufferVisualizer
	{
	public:
		DepthBufferVisualizer();
		~DepthBufferVisualizer();

		void Initialize();
		void CopyCurrentDepthBuffer(uint32_t p_width, uint32_t p_height);
		void Cleanup();

		uint32_t GetDepthTexture() const { return m_depthTextureID; }

		bool IsInitialized() const { return m_initialized; }

	private:
		uint32_t m_depthTextureID;
		bool m_initialized;
	};

	// Guilherme's exact function signature
	void CopyDepthBuffer(uint32_t p_textureID, int p_x, int p_y, int p_width, int p_height);
}
