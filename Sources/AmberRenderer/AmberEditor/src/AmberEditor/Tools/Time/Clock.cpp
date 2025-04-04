#include "AmberEditor/Tools/Time/Clock.h"

void AmberEditor::Tools::Time::Clock::Initialize()
{
	deltaTime = 0.0f;

	currentTime = std::chrono::steady_clock::now();
	lastTime = currentTime;

	isInitialized = true;
}

void AmberEditor::Tools::Time::Clock::Update()
{
	currentTime = std::chrono::steady_clock::now();
	elapsedTime = currentTime - lastTime;

	lastTime = currentTime;

	if (isInitialized)
	{
		deltaTime = elapsedTime.count() > 0.1 ? 0.1f : static_cast<float>(elapsedTime.count());

		timeSinceStart += deltaTime * timeScale;
	}
	else
	{
		Initialize();
	}
}

float AmberEditor::Tools::Time::Clock::GetFrameRate() const
{
	return 1.0f / (deltaTime);
}

float AmberEditor::Tools::Time::Clock::GetDeltaTime() const
{
	return deltaTime * timeScale;
}
