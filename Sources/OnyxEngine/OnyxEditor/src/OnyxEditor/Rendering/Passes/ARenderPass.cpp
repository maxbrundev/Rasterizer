#include "OnyxEditor/Rendering/Passes/ARenderPass.h"

#include "OnyxEditor/Rendering/Debug/FrameDebuggerMacros.h"

OnyxEditor::Rendering::Passes::ARenderPass::ARenderPass(Renderer& p_renderer) : m_renderer(p_renderer)
{
}

void OnyxEditor::Rendering::Passes::ARenderPass::SetEnabled(bool p_value)
{
	m_isEnabled = p_value;
}

bool OnyxEditor::Rendering::Passes::ARenderPass::IsEnabled() const
{
	return m_isEnabled;
}