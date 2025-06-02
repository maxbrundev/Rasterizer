#include "AmberEditor/Rendering/Passes/ARenderPass.h"

AmberEditor::Rendering::Passes::ARenderPass::ARenderPass(Renderer& p_renderer) : m_renderer(p_renderer)
{
}

void AmberEditor::Rendering::Passes::ARenderPass::SetEnabled(bool p_value)
{
	m_isEnabled = p_value;
}

bool AmberEditor::Rendering::Passes::ARenderPass::IsEnabled() const
{
	return m_isEnabled;
}