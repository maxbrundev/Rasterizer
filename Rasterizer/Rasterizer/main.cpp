#define SDL_MAIN_HANDLED

#include "Settings/DriverSettings.h"

#include "Context/Device.h"
#include "Context/Window.h"

#include "Core/Renderer.h"

#include "Core/Rasterizer.h"

int main()
{
	Settings::WindowSettings windowSettings;
	windowSettings.title = "test";
	windowSettings.width = 1280;
	windowSettings.height = 720;
	
	Settings::DriverSettings driverSettings;
	driverSettings.vsync = true;

	Context::Device device;
	Context::Window window(device, windowSettings);

	Core::Renderer renderer(window, driverSettings);

	Data::Color backGround(0, 0, 0);

	Core::Rasterizer rasterRizer(renderer, window.GetSize().first, window.GetSize().second);

	rasterRizer.SetScissorRect(0, 0, window.GetSize().first, window.GetSize().second);

	Geometry::Vertex vertex0;
	Geometry::Vertex vertex1;
	Geometry::Vertex vertex2;
	
	vertex0.x = (1280 / 2);
	vertex0.y = 100;
	vertex0.color.r = 255;
	vertex0.color.g = 0;
	vertex0.color.b = 0;
	
	vertex1.x = (1280 / 2) - 200;
	vertex1.y = 400;
	vertex1.color.r = 0;
	vertex1.color.g = 255;
	vertex1.color.b = 0;
	
	vertex2.x = (1280 / 2) + 200;
	vertex2.y = 400;
	vertex2.color.r = 0;
	vertex2.color.g = 0;
	vertex2.color.b = 255;

	while (window.IsActive()) 
	{
		device.HandleEvent();

		rasterRizer.GetTextureBuffer().Clear(backGround);

		renderer.RenderClear();

		rasterRizer.DrawTriangle(vertex0, vertex1, vertex2);

		rasterRizer.GetTextureBuffer().SendDataToGPU();

		renderer.RenderCopy(rasterRizer.GetTextureBuffer());
		renderer.RenderPresent();
	}
}