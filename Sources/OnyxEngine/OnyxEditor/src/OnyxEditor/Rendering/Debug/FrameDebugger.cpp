#include "OnyxEditor/Rendering/Debug/FrameDebugger.h"
#include <iostream>

void OnyxEditor::Rendering::Debug::FrameDebugger::StartCapture()
{
	Clear();
	m_isCapturingFrame = true;
}

void OnyxEditor::Rendering::Debug::FrameDebugger::StopCapture()
{
	m_isCapturingFrame = false;
}

void OnyxEditor::Rendering::Debug::FrameDebugger::CheckStartFrameCapture()
{
	if (!m_isCapturingFrame) 
		return;

	Clear();
}

void OnyxEditor::Rendering::Debug::FrameDebugger::CheckEndFrameCapture()
{
	if (!m_isCapturingFrame)
		return;

	if (!m_steps.empty())
	{
		m_selectedStep = m_steps.size() - 1;
	}

	m_isCapturingFrame = false;
}

void OnyxEditor::Rendering::Debug::FrameDebugger::RecordClear(const std::string& p_description, uint32_t p_capturedTextureID)
{
	if (!m_isCapturingFrame)
		return;

	FrameStep step;
	step.Description = p_description;
	step.PassName = "Clear";
	step.CapturedTextureID = p_capturedTextureID;
	m_steps.push_back(step);
}

void OnyxEditor::Rendering::Debug::FrameDebugger::RecordDrawCall(const std::string& p_description, uint32_t p_triangles, uint32_t p_vertices, const std::string& p_materialName, const std::string& p_passName, uint32_t p_capturedTextureID)
{
	if (!m_isCapturingFrame)
		return;

	FrameStep step;
	step.Description = p_description;
	step.TriangleCount = p_triangles;
	step.VertexCount = p_vertices;
	step.MaterialName = p_materialName;
	step.PassName = p_passName.empty() ? "Draw" : p_passName;
	step.CapturedTextureID = p_capturedTextureID;
	m_steps.push_back(step);
}

void OnyxEditor::Rendering::Debug::FrameDebugger::RecordSetRenderTarget(const std::string& p_description, uint32_t p_capturedTextureID)
{
	if (!m_isCapturingFrame)
		return;

	FrameStep step;
	step.Description = p_description;
	step.PassName = "SetRenderTarget";
	step.CapturedTextureID = p_capturedTextureID;
	m_steps.push_back(step);
}

void OnyxEditor::Rendering::Debug::FrameDebugger::SelectStep(uint32_t p_stepIndex)
{
	if (p_stepIndex >= m_steps.size()) 
		return;

	m_selectedStep = p_stepIndex;
}

void OnyxEditor::Rendering::Debug::FrameDebugger::NextStep()
{
	if (m_selectedStep < m_steps.size() - 1)
	{
		SelectStep(m_selectedStep + 1);
	}
}

void OnyxEditor::Rendering::Debug::FrameDebugger::PrevStep()
{
	if (m_selectedStep > 0)
	{
		SelectStep(m_selectedStep - 1);
	}
}

void OnyxEditor::Rendering::Debug::FrameDebugger::FirstStep()
{
	if (!m_steps.empty())
	{
		SelectStep(0);
	}
}

void OnyxEditor::Rendering::Debug::FrameDebugger::LastStep()
{
	if (!m_steps.empty())
	{
		SelectStep(m_steps.size() - 1);
	}
}

uint32_t OnyxEditor::Rendering::Debug::FrameDebugger::GetSelectedStep() const
{
	return m_selectedStep;
}

uint32_t OnyxEditor::Rendering::Debug::FrameDebugger::GetStepsCount() const
{
	return static_cast<uint32_t>(m_steps.size());
}

bool OnyxEditor::Rendering::Debug::FrameDebugger::HasCapturedSteps() const
{
	return !m_steps.empty();
}

const std::vector<OnyxEditor::Rendering::Debug::FrameStep>& OnyxEditor::Rendering::Debug::FrameDebugger::GetSteps() const
{
	return m_steps;
}

const OnyxEditor::Rendering::Debug::FrameStep* OnyxEditor::Rendering::Debug::FrameDebugger::GetCurrentStep() const
{
	if (m_selectedStep >= m_steps.size()) 
		return nullptr;

	return &m_steps[m_selectedStep];
}

uint32_t OnyxEditor::Rendering::Debug::FrameDebugger::GetCurrentStepTextureID() const
{
	if (m_selectedStep >= m_steps.size()) 
		return 0;

	return m_steps[m_selectedStep].CapturedTextureID;
}

void OnyxEditor::Rendering::Debug::FrameDebugger::Clear()
{
	m_selectedStep = 0;
	m_steps.clear();
}
