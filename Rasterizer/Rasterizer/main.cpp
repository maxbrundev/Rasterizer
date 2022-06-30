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

	//LINE LEFT TO RIGHT
	Geometry::Vertex vertex3;
	Geometry::Vertex vertex4;
	
	vertex3.x = 0;
	vertex3.y = 500;
	vertex3.color.r = 255;
	vertex3.color.g = 0;
	vertex3.color.b = 0;

	vertex4.x = 500;
	vertex4.y = 500;
	vertex4.color.r = 0;
	vertex4.color.g = 255;
	vertex4.color.b = 0;

	//LINE TOP TO BOTTOM
	Geometry::Vertex vertex5;
	Geometry::Vertex vertex6;
	vertex5.x = 250;
	vertex5.y = 500;
	vertex5.color.r = 255;
	vertex5.color.g = 0;
	vertex5.color.b = 0;

	vertex6.x = 250;
	vertex6.y = 700;
	vertex6.color.r = 0;
	vertex6.color.g = 255;
	vertex6.color.b = 0;

	//LINE BOTTOM TO TOP
	Geometry::Vertex vertex7;
	Geometry::Vertex vertex8;
	vertex7.x = 250;
	vertex7.y = 500;
	vertex7.color.r = 255;
	vertex7.color.g = 0;
	vertex7.color.b = 0;
	
	vertex8.x = 250;
	vertex8.y = 300;
	vertex8.color.r = 0;
	vertex8.color.g = 255;
	vertex8.color.b = 0;

	//LINE RIGHT TO LEFT
	Geometry::Vertex vertex9;
	Geometry::Vertex vertex10;
	vertex9.x = 500;
	vertex9.y = 400;
	vertex9.color.r = 255;
	vertex9.color.g = 0;
	vertex9.color.b = 0;

	vertex10.x = 0;
	vertex10.y = 400;
	vertex10.color.r = 0;
	vertex10.color.g = 255;
	vertex10.color.b = 0;

	while (window.IsActive()) 
	{
		device.HandleEvent();

		rasterRizer.GetTextureBuffer().Clear(backGround);

		renderer.RenderClear();

		rasterRizer.DrawTriangle(vertex0, vertex1, vertex2);

		rasterRizer.DrawWireFrameTriangle({ vertex0.x + 400, vertex0.y, vertex0.color}, { vertex1.x + 400, vertex1.y,  vertex1.color }, { vertex2.x + 400, vertex2.y,  vertex2.color });

		rasterRizer.DrawLine(vertex3, vertex4);
		rasterRizer.DrawLine(vertex5, vertex6);
		rasterRizer.DrawLine(vertex7, vertex8);
		rasterRizer.DrawLine(vertex9, vertex10);

		rasterRizer.GetTextureBuffer().SendDataToGPU();

		renderer.RenderCopy(rasterRizer.GetTextureBuffer());
		renderer.RenderPresent();
	}
}