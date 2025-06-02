#include "AmberEditor/Rendering/Features/ARenderFeature.h"

AmberEditor::Rendering::Features::ARenderFeature::ARenderFeature(Renderer& p_renderer) : m_renderer(p_renderer)
{
}

void AmberEditor::Rendering::Features::ARenderFeature::SetEnabled(bool p_value)
{
	m_isEnabled = p_value;
}

bool AmberEditor::Rendering::Features::ARenderFeature::IsEnabled() const
{
	return m_isEnabled;
}