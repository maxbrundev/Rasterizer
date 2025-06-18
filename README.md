<p align="center" style="margin-bottom: 0;">
  <img src="https://github.com/user-attachments/assets/1e35c508-19b7-438e-b4b4-05a6911461c6" alt="Logo" />
</p>

<p align="center">
  <a href="#video-and-screenshots">Screenshots</a> |
  <a href="#ambergl-api">AmberGL API</a>
<br/>

# Realtime CPU Software Renderer

# Description
A CPU-based software renderer designed to explore low-level rendering techniques and principles. This project implements various rasterization features without relying on hardware acceleration, making it a great educational and experimentation project for graphics programming.

# Features
- Triangle Rasterization
- Line Rasterization
- Point
- Texture Mapping
- Clipping
- Mipmapping based on texture derivates
- MSAA
- Vertex pass
- Fragment pass
- Depth Writing
- Depth Testing
- Shadow Mapping 
- OBJ Parsing  

| Primitive Mode | Polygone Mode | Clipping | Model Rasterization | Multisample Anti-Aliasing |
| :------------------------: | :------------------------: | :------------------------: | :------------------------: | :------------------------: |
| ![primitiveMode](https://github.com/user-attachments/assets/da984a4a-5f58-4716-81a0-d8ab13fef2e9) | ![polygoneMode](https://github.com/user-attachments/assets/7f43f69a-7621-4441-b8db-8b63b1a9a3ad) | ![clipping](https://github.com/user-attachments/assets/e0f2d8fb-1a8f-4ced-aae6-053245e33563) | ![modelRasterization](https://github.com/user-attachments/assets/6555c768-3f92-42da-aa37-8c30ec42678c) | ![msaa](https://github.com/user-attachments/assets/1871223b-d290-43ea-95c6-7e9f738afe30) |


| Mipmapping | Texture Coordinates Derivates | Shadow Mapping | Complete Rasterization Feature Set |
| :------------------------: | :------------------------: | :------------------------: | :------------------------: |
| ![Texture Sampling Technique](https://github.com/user-attachments/assets/50b01b40-f404-4209-8347-0063fbda3c9d) | ![Mipmap Level Detail](https://github.com/user-attachments/assets/5464680e-f90a-4d3c-80df-5717f84f0375) | ![Shadow Mapping](https://github.com/user-attachments/assets/c6dcfb1d-b6bd-4d34-8e0b-25655653ace8) ![simpleShadow](https://github.com/user-attachments/assets/b1209db6-421e-456a-b4f3-b51a14ddf993) | ![demoTest](https://github.com/user-attachments/assets/0ff270e0-2686-47db-aa85-ab7bf383bd8e) |

# AmberGL API
An OpenGL-inspired API that encapsulates its own Rendering State and manages internal buffers to provide a streamlined interface for Drawing, handling Buffers, and processing Shaders.

## Setting Up the Renderer
Initialize the AmberGL rendering system and configure the basic rendering state:

```cpp
// Initialize the renderer with width and height
AmberGL::Initialize(800, 600);

// Configure multisampling for anti-aliasing
AmberGL::WindowHint(AGL_SAMPLES, 4); // 4x multisampling
AmberGL::Enable(AGL_MULTISAMPLE);    // Enable multisampling

// Set up the rendering state
AmberGL::Enable(AGL_DEPTH_TEST);  // Enable depth testing
AmberGL::Enable(AGL_DEPTH_WRITE); // Enable depth writing
AmberGL::Enable(AGL_CULL_FACE);   // Enable face culling
AmberGL::CullFace(AGL_BACK);      // Cull back faces

// Set viewport dimensions
AmberGL::Viewport(0, 0, 800, 600);

// Set clear color
AmberGL::ClearColor(0.2f, 0.3f, 0.3f, 1.0f);
```

### Drawing a Simple Triangle
Basic example of setting up a vertex buffer and drawing a triangle:

```cpp
// Define triangle vertices
std::vector<Geometry::Vertex> vertices = {
    Geometry::Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    Geometry::Vertex(glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    Geometry::Vertex(glm::vec3( 0.0f,  0.5f, 0.0f), glm::vec2(0.5f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f))
};

// Create and bind a vertex array object
uint32_t VAO;
AmberGL::GenVertexArrays(1, &VAO);
AmberGL::BindVertexArray(VAO);

// Create and bind a vertex buffer
uint32_t VBO;
AmberGL::GenBuffers(1, &VBO);
AmberGL::BindBuffer(AGL_ARRAY_BUFFER, VBO);
AmberGL::BufferData(AGL_ARRAY_BUFFER, vertices.size() * sizeof(Geometry::Vertex), vertices.data());

// Create program and shader
uint32_t program = AmberGL::CreateProgram();
AmberGL::AttachShader(program, shaderInstance);

// Main rendering loop
while (running) {
    // Clear the screen
    AmberGL::Clear(AGL_COLOR_BUFFER_BIT | AGL_DEPTH_BUFFER_BIT);
    
    // Calculate transformation matrices
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = camera.GetProjectionMatrix();
    
    // Set shader uniforms
    shaderInstance->SetUniform("u_Model", model);
    shaderInstance->SetUniform("u_View", view);
    shaderInstance->SetUniform("u_Projection", projection);
    
    // Use the program
    AmberGL::UseProgram(program);
    
    // Bind the VAO and draw
    AmberGL::BindVertexArray(VAO);
    AmberGL::DrawArrays(AGL_TRIANGLES, 0, 3);
    AmberGL::BindVertexArray(0);
}

// Cleanup
AmberGL::DeleteProgram(program);
AmberGL::DeleteBuffers(1, &VBO);
AmberGL::DeleteVertexArrays(1, &VAO);
```

### Indexed Rendering
Optimize rendering by using index buffers:

```cpp
// Define vertices and indices for a square (two triangles)
std::vector<Geometry::Vertex> vertices = {
    Geometry::Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    Geometry::Vertex(glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    Geometry::Vertex(glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    Geometry::Vertex(glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f))
};

std::vector<uint32_t> indices = {
    0, 1, 2,  // First triangle
    2, 3, 0   // Second triangle
};

// Create and bind VAO
uint32_t VAO;
AmberGL::GenVertexArrays(1, &VAO);
AmberGL::BindVertexArray(VAO);

// Create and bind VBO
uint32_t VBO;
AmberGL::GenBuffers(1, &VBO);
AmberGL::BindBuffer(AGL_ARRAY_BUFFER, VBO);
AmberGL::BufferData(AGL_ARRAY_BUFFER, vertices.size() * sizeof(Geometry::Vertex), vertices.data());

// Create and bind EBO (Element Buffer Object)
uint32_t EBO;
AmberGL::GenBuffers(1, &EBO);
AmberGL::BindBuffer(AGL_ELEMENT_ARRAY_BUFFER, EBO);
AmberGL::BufferData(AGL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data());

// Create program and attach shader
uint32_t program = AmberGL::CreateProgram();
AmberGL::AttachShader(program, shaderInstance);

// During rendering:
// Set transformation matrices
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = camera.GetViewMatrix();
glm::mat4 projection = camera.GetProjectionMatrix();

// Set shader uniforms
shaderInstance->SetUniform("u_Model", model);
shaderInstance->SetUniform("u_View", view);
shaderInstance->SetUniform("u_Projection", projection);

// Use program and draw
AmberGL::UseProgram(program);
AmberGL::BindVertexArray(VAO);
AmberGL::DrawElements(AGL_TRIANGLES, indices.size());
AmberGL::BindVertexArray(0);
```

### Textured Cube
Create a textured 3D cube:

```cpp
// Define cube vertices with positions, UVs, and normals
std::vector<Geometry::Vertex> cubeVertices = {
    // Front face
    Geometry::Vertex(glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    Geometry::Vertex(glm::vec3( 1.0f, -1.0f,  1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    Geometry::Vertex(glm::vec3( 1.0f,  1.0f,  1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    Geometry::Vertex(glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    // Back face
    Geometry::Vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
    Geometry::Vertex(glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
    Geometry::Vertex(glm::vec3( 1.0f,  1.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
    Geometry::Vertex(glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
    // Top face
    Geometry::Vertex(glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
    Geometry::Vertex(glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
    Geometry::Vertex(glm::vec3( 1.0f,  1.0f,  1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
    Geometry::Vertex(glm::vec3( 1.0f,  1.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
    // Bottom face
    Geometry::Vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    Geometry::Vertex(glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    Geometry::Vertex(glm::vec3( 1.0f, -1.0f,  1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    Geometry::Vertex(glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    // Right face
    Geometry::Vertex(glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
    Geometry::Vertex(glm::vec3( 1.0f,  1.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
    Geometry::Vertex(glm::vec3( 1.0f,  1.0f,  1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
    Geometry::Vertex(glm::vec3( 1.0f, -1.0f,  1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
    // Left face
    Geometry::Vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f)),
    Geometry::Vertex(glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f)),
    Geometry::Vertex(glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)),
    Geometry::Vertex(glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f))
};

// Define cube indices
std::vector<uint32_t> cubeIndices = {
    0, 1, 2, 2, 3, 0,       // Front face
    4, 5, 6, 6, 7, 4,       // Back face
    8, 9, 10, 10, 11, 8,    // Top face
    12, 13, 14, 14, 15, 12, // Bottom face
    16, 17, 18, 18, 19, 16, // Right face
    20, 21, 22, 22, 23, 20  // Left face
};

// Create and bind a vertex array object
uint32_t cubeVAO;
AmberGL::GenVertexArrays(1, &cubeVAO);
AmberGL::BindVertexArray(cubeVAO);

// Create and bind a vertex buffer
uint32_t cubeVBO;
AmberGL::GenBuffers(1, &cubeVBO);
AmberGL::BindBuffer(AGL_ARRAY_BUFFER, cubeVBO);
AmberGL::BufferData(AGL_ARRAY_BUFFER, cubeVertices.size() * sizeof(Geometry::Vertex), cubeVertices.data());

// Create and bind element buffer object
uint32_t cubeEBO;
AmberGL::GenBuffers(1, &cubeEBO);
AmberGL::BindBuffer(AGL_ELEMENT_ARRAY_BUFFER, cubeEBO);
AmberGL::BufferData(AGL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(uint32_t), cubeIndices.data());

// Create and load a texture
uint32_t texture;
AmberGL::GenTextures(1, &texture);
AmberGL::BindTexture(AGL_TEXTURE_2D, texture);

// Set texture parameters
AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MIN_FILTER, AGL_LINEAR);
AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MAG_FILTER, AGL_LINEAR);
AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_WRAP_S, AGL_REPEAT);
AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_WRAP_T, AGL_REPEAT);

// Load texture data
uint8_t* data;
uint32_t width, height;
loadTextureData("texture.png", data, width, height);

AmberGL::TexImage2D(AGL_TEXTURE_2D, 0, AGL_RGBA8, width, height, 0, AGL_RGBA8, AGL_UNSIGNED_BYTE, data);
AmberGL::GenerateMipmap(AGL_TEXTURE_2D);

// Create program and attach shader
uint32_t cubeProgram = AmberGL::CreateProgram();
AmberGL::AttachShader(cubeProgram, cubeShaderInstance);

// Rendering loop
while (running) {
    AmberGL::Clear(AGL_COLOR_BUFFER_BIT | AGL_DEPTH_BUFFER_BIT);
    
    // Model matrix - static cube
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = camera.GetProjectionMatrix();
    
    // Set shader uniforms
    cubeShaderInstance->SetUniform("u_Model", model);
    cubeShaderInstance->SetUniform("u_View", view);
    cubeShaderInstance->SetUniform("u_Projection", projection);
    cubeShaderInstance->SetUniform("u_DiffuseMap", 0); // Texture unit 0
    cubeShaderInstance->SetUniform("u_ViewPos", cameraPosition);
    
    // Use the program
    AmberGL::UseProgram(cubeProgram);
    
    // Bind texture
    AmberGL::ActiveTexture(AGL_TEXTURE0);
    AmberGL::BindTexture(AGL_TEXTURE_2D, texture);
    
    // Draw the cube
    AmberGL::BindVertexArray(cubeVAO);
    AmberGL::DrawElements(AGL_TRIANGLES, cubeIndices.size());
    AmberGL::BindVertexArray(0);
}

// Cleanup
AmberGL::DeleteTextures(1, &texture);
AmberGL::DeleteBuffers(1, &cubeVBO);
AmberGL::DeleteBuffers(1, &cubeEBO);
AmberGL::DeleteVertexArrays(1, &cubeVAO);
AmberGL::DeleteProgram(cubeProgram);
```

### Framebuffer Operations
Creating and using framebuffers for post-processing effects:

```cpp
// Create a framebuffer object for off-screen rendering
uint32_t FBO;
AmberGL::GenFramebuffers(1, &FBO);
AmberGL::BindFramebuffer(AGL_FRAMEBUFFER, FBO);

// Create a texture for the color attachment
uint32_t colorTexture;
AmberGL::GenTextures(1, &colorTexture);
AmberGL::BindTexture(AGL_TEXTURE_2D, colorTexture);
AmberGL::TexImage2D(AGL_TEXTURE_2D, 0, AGL_RGBA8, 1024, 1024, 0, AGL_RGBA8, AGL_UNSIGNED_BYTE, nullptr);
AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MIN_FILTER, AGL_LINEAR);
AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MAG_FILTER, AGL_LINEAR);
AmberGL::FramebufferTexture2D(AGL_FRAMEBUFFER, AGL_COLOR_ATTACHMENT, AGL_TEXTURE_2D, colorTexture, 0);

// Create a renderbuffer object for depth testing
uint32_t RBO;
AmberGL::GenRenderbuffers(1, &RBO);
AmberGL::BindRenderbuffer(AGL_RENDERBUFFER, RBO);
AmberGL::RenderbufferStorage(AGL_RENDERBUFFER, AGL_DEPTH_COMPONENT, 1024, 1024);
AmberGL::FramebufferRenderbuffer(AGL_FRAMEBUFFER, AGL_DEPTH_ATTACHMENT, AGL_RENDERBUFFER, RBO);

// Unbind the framebuffer
AmberGL::BindFramebuffer(AGL_FRAMEBUFFER, 0);

// Create programs for scene rendering and post-processing
uint32_t sceneProgram = AmberGL::CreateProgram();
AmberGL::AttachShader(sceneProgram, sceneShaderInstance);

uint32_t postProcessProgram = AmberGL::CreateProgram();
AmberGL::AttachShader(postProcessProgram, postProcessShaderInstance);

// Prepare post-process quad vertices
std::vector<Geometry::Vertex> quadVertices = {
    Geometry::Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    Geometry::Vertex(glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    Geometry::Vertex(glm::vec3( 1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    Geometry::Vertex(glm::vec3( 1.0f,  1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f))
};

// Create and bind VAO for quad
uint32_t quadVAO;
AmberGL::GenVertexArrays(1, &quadVAO);
AmberGL::BindVertexArray(quadVAO);

// Create VBO for quad
uint32_t quadVBO;
AmberGL::GenBuffers(1, &quadVBO);
AmberGL::BindBuffer(AGL_ARRAY_BUFFER, quadVBO);
AmberGL::BufferData(AGL_ARRAY_BUFFER, quadVertices.size() * sizeof(Geometry::Vertex), quadVertices.data());
AmberGL::BindVertexArray(0);

// Main rendering loop
while (running) {
    // ======== First pass: Render scene to framebuffer ========
    AmberGL::BindFramebuffer(AGL_FRAMEBUFFER, FBO);
    AmberGL::Viewport(0, 0, 1024, 1024);
    AmberGL::Clear(AGL_COLOR_BUFFER_BIT | AGL_DEPTH_BUFFER_BIT);
    
    // Setup scene rendering
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = camera.GetProjectionMatrix();
    
    // Set scene shader uniforms
    sceneShaderInstance->SetUniform("u_Model", model);
    sceneShaderInstance->SetUniform("u_View", view);
    sceneShaderInstance->SetUniform("u_Projection", projection);
    sceneShaderInstance->SetUniform("u_DiffuseMap", 0);
    
    // Use scene program and bind textures
    AmberGL::UseProgram(sceneProgram);
    AmberGL::ActiveTexture(AGL_TEXTURE0);
    AmberGL::BindTexture(AGL_TEXTURE_2D, sceneTexture);
    
    // Render scene objects
    AmberGL::BindVertexArray(sceneVAO);
    AmberGL::DrawElements(AGL_TRIANGLES, sceneIndexCount);
    AmberGL::BindVertexArray(0);
    
    // ======== Second pass: Apply post-processing effect ========
    AmberGL::BindFramebuffer(AGL_FRAMEBUFFER, 0);
    AmberGL::Viewport(0, 0, 800, 600);
    AmberGL::Clear(AGL_COLOR_BUFFER_BIT);
    
    // Set post-processing uniforms
    float time = GetCurrentTime();  // Get current time for animated effects
    postProcessShaderInstance->SetUniform("u_ScreenTexture", 0);
    postProcessShaderInstance->SetUniform("u_Time", time);
    postProcessShaderInstance->SetUniform("u_Effect", 1);  // 1 = grayscale, 2 = invert, 3 = blur, etc.
    
    // Use post-processing program
    AmberGL::UseProgram(postProcessProgram);
    
    // Bind the framebuffer's color texture
    AmberGL::ActiveTexture(AGL_TEXTURE0);
    AmberGL::BindTexture(AGL_TEXTURE_2D, colorTexture);
    
    // Render the quad with post-processing effect applied
    AmberGL::BindVertexArray(quadVAO);
    AmberGL::DrawArrays(AGL_TRIANGLE_STRIP, 0, 4);
    AmberGL::BindVertexArray(0);
}

// Cleanup resources
AmberGL::DeleteTextures(1, &colorTexture);
AmberGL::DeleteRenderbuffers(1, &RBO);
AmberGL::DeleteFramebuffers(1, &FBO);
AmberGL::DeleteBuffers(1, &quadVBO);
AmberGL::DeleteVertexArrays(1, &quadVAO);
AmberGL::DeleteProgram(sceneProgram);
AmberGL::DeleteProgram(postProcessProgram);
```

# Roadmap
- **UI Integration:**  
  Implement a dedicated Panel to allow realtime control over:
  - Rasterization settings
  - Render passes
  - Scene management  
  ...

- **Global Optimization:**
  - SIMD
  - Tile Based Rasterization

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

**Sponza rendered with shadow mapping and MSAA x16**
![sponzaShadows](https://github.com/user-attachments/assets/c6dcfb1d-b6bd-4d34-8e0b-25655653ace8)

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
