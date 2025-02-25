## Realtime CPU Software Rasterizer

![rasterizer](https://github.com/user-attachments/assets/5e4d9c2a-f825-4428-9464-f95a6d26f1dc)

# Description
A CPU-based software rasterizer designed to explore low-level rendering techniques and principles. This project implements various rasterization features without relying on hardware acceleration, making it a great educational and experimentation project for graphics programming.

# Features
- Triangle Rasterization
- Line Rasterization
- Texture Mapping
- Clipping
- Mipmapping
- MSAA
- Vertex pass
- Fragment pass
- Depth Writing
- Depth Testing
- OBJ Parsing

# GL Rasterizer API
An OpenGL-inspired API that encapsulates its own Rendering State and manages internal buffers to provide a streamlined interface for Drawing, handling Buffers, and processing Shaders.
```cpp
uint32_t VAO;
uint32_t VBO;
uint32_t EBO;

std::vector<Geometry::Vertex> vertices = {
	{{-1.0f, 0.0f, -1.0f}},
	{{-1.0f, 0.0f, 1.0f}},
	{{1.0f, 0.0f, 1.0f}},
	{{1.0f, 0.0f, -1.0f}}
};

std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };

GLRasterizer::GenBuffers(1, &VBO);
GLRasterizer::BindBuffer(GLR_ARRAY_BUFFER, VBO);
GLRasterizer::BufferData(GLR_ARRAY_BUFFER,
	vertices.size() * sizeof(Geometry::Vertex),
	vertices.data());

GLRasterizer::GenBuffers(1, &EBO);
GLRasterizer::BindBuffer(GLR_ELEMENT_ARRAY_BUFFER, EBO);
GLRasterizer::BufferData(GLR_ELEMENT_ARRAY_BUFFER,
	indices.size() * sizeof(uint32_t),
	indices.data());

GLRasterizer::GenVertexArrays(1, &VAO);
GLRasterizer::BindVertexArray(VAO);

GLRasterizer::BindBuffer(GLR_ARRAY_BUFFER, VBO);
GLRasterizer::BindBuffer(GLR_ELEMENT_ARRAY_BUFFER, EBO);

GLRasterizer::BindVertexArray(0);

GLRasterizer::UseProgram(shaderInstance);
GLRasterizer::BindVertexArray(VAO);
GLRasterizer::DrawElements(GLR_TRIANGLES, static_cast<uint32_t>(indices.size()));
GLRasterizer::BindVertexArray(0);
```

# Roadmap
- **Rendering Enhancements:**  
  - **Renderer:**  
    The current Renderer is minimal; to enhance modularity and extensibility, a rework is required to support a stateful pipeline and accommodate multiple render passes.
  - **Anti-aliasing:**  
    Refine the existing MSAA implementation and explore additional multi-sampling techniques (e.g., FXAA, SMAA) to effectively reduce aliasing artifacts while maintaining performance.
  - **Mipmapping:**  
    Optimize the mipmap generation algorithm for improved texture sampling and memory efficiency, and rework the adaptive level-of-detail (LOD) system to increase realtime performance and visual quality.
  - **Shadow Mapping:**  
    Implement a robust shadow mapping pipeline that addresses common challenges (e.g., shadow acne, depth biasing) to achieve dynamic shadows.

- **Rework Architecture:**  
  Overhaul the current architecture to develop a modular, game engine–style framework. This includes providing an OpenGL-like API for rasterizer-related functions, thereby facilitating easier integration and future expansion.

- **UI Integration:**  
  Implement a dedicated Panel to allow realtime control over:
  - Rasterization settings
  - Render passes
  - Scene management  
  ...

- **Global Optimization:**  
Global optimization pass to improve overall performance and efficiency.

# Software
- Visual Studio 2022

# Dependencies
- SDL2 (Windowing and inputs)
- GLM (Mathematics)
- stb_image (Image Loader)
- Premake5 (Project generation)

# Compiling sources
Premake5 is used to generate project files.
To generate the project, execute `GenerateProject.bat`. By default, `GenerateProject.bat` will generate project files for Visual Studio 2022. If you want to use another version of Visual Studio you can execute `GenerateProject.bat` from the command line with the Visual Studio version as argument. (ex: `.\GeneratedProject.bat vs2019`)

# Video and Screenshots
[![cppRasterizer](https://img.youtube.com/vi/UjUxgVGn0rU/0.jpg)](https://www.youtube.com/watch?v=UjUxgVGn0rU)

**Sponza rendered MSAA x8 thanks to the WIP resources management rework**
![sponzaRasterizer](https://github.com/user-attachments/assets/e8228b72-4a93-4125-a2f8-388d8e82d5be)

**Polygon Rasterization Mode**
![primitiveMode](https://github.com/user-attachments/assets/d3d75daa-4ea8-4e84-9cc6-651b9d64f77f)

**Texture Filtering**
![TextureFilter](https://github.com/maxbrundev/Rasterizer/assets/32653095/b9a5a633-657d-4205-8575-53fdfb76b4b3)
*Left Linear, Right Nearest.*

**Mipmapping**
![mipmaps](https://github.com/maxbrundev/Rasterizer/assets/32653095/2aae379f-106f-4db3-85b7-4c7882392c71)
*Left mipmaps off, Right mipmaps on.*

**Clipping**

![clipping](https://github.com/user-attachments/assets/b2ac8825-3ba7-47fa-930d-39986438a02c)  
*Frustum plane distances have been reduced to highlight clipping*

# Licence
This project is licenced under an MIT Licence.
