#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include "OnyxEditor/Rendering/Debug/FrameStep.h"

namespace OnyxEditor::Rendering::Debug
{
	class FrameDebugger
	{
	public:
		FrameDebugger() = default;
		~FrameDebugger() = default;

		void StartCapture();
		void StopCapture();
		bool IsCapturingFrame() const { return m_isCapturingFrame; }

		void CheckStartFrameCapture();
		void CheckEndFrameCapture();

		void RecordClear(const std::string& p_description, uint32_t p_capturedTextureID);
		void RecordDrawCall(const std::string& p_description, uint32_t p_triangles, uint32_t p_vertices, const std::string& p_materialName, const std::string& p_passName, uint32_t p_capturedTextureID);
		void RecordSetRenderTarget(const std::string& p_description, uint32_t p_capturedTextureID);

		void SelectStep(uint32_t p_stepIndex);
		void NextStep();
		void PrevStep();
		void FirstStep();
		void LastStep();

		uint32_t GetSelectedStep() const;
		uint32_t GetStepsCount() const;
		bool HasCapturedSteps() const;

		const std::vector<FrameStep>& GetSteps() const;

		const FrameStep* GetCurrentStep() const;
		uint32_t GetCurrentStepTextureID() const;

		void Clear();

	private:
		bool m_isCapturingFrame = false;
		
		std::vector<FrameStep> m_steps;
		uint32_t m_selectedStep = 0;
	};
}
