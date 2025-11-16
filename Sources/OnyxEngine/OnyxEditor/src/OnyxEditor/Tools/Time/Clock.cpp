#include "OnyxEditor/Tools/Time/Clock.h"

void OnyxEditor::Tools::Time::Clock::Initialize()
{
	deltaTime = 0.0f;

	currentTime = std::chrono::steady_clock::now();
	lastTime = currentTime;

	isInitialized = true;
}

void OnyxEditor::Tools::Time::Clock::Update()
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

float OnyxEditor::Tools::Time::Clock::GetFrameRate() const
{
	return 1.0f / (deltaTime);
}

float OnyxEditor::Tools::Time::Clock::GetDeltaTime() const
{
	return deltaTime * timeScale;
}
