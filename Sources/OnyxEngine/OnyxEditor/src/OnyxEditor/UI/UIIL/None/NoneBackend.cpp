#include "OnyxEditor/UI/UIIL/None/NoneBackend.h"

template <>
bool OnyxEditor::UI::UIIL::NoneBackend::Initialize(void* p_window, void* p_context)
{
	m_context.IsInitialized = true;
	return true;
}

template <>
void OnyxEditor::UI::UIIL::NoneBackend::Shutdown()
{
	m_context.IsInitialized = false;
}

template <>
void OnyxEditor::UI::UIIL::NoneBackend::NewFrame()
{
}

template <>
void OnyxEditor::UI::UIIL::NoneBackend::RenderDrawData()
{
}

template <>
std::string OnyxEditor::UI::UIIL::NoneBackend::GetBackendName() const
{
	return "None";
}
