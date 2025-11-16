#include "OnyxEditor/Rendering/Features/ARenderFeature.h"

OnyxEditor::Rendering::Features::ARenderFeature::ARenderFeature(Renderer& p_renderer) : m_renderer(p_renderer)
{
}

void OnyxEditor::Rendering::Features::ARenderFeature::SetEnabled(bool p_value)
{
	m_isEnabled = p_value;
}

bool OnyxEditor::Rendering::Features::ARenderFeature::IsEnabled() const
{
	return m_isEnabled;
}