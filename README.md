<p align="center" style="margin-bottom: 0;">
  <img src="https://github.com/user-attachments/assets/5e4d9c2a-f825-4428-9464-f95a6d26f1dc" alt="Logo" />
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
- Texture Mapping
- Clipping
- Mipmapping
- MSAA
- Vertex pass
- Fragment pass
- Depth Writing
- Depth Testing
- Shadow Mapping 
- OBJ Parsing  
 
![shadows](https://github.com/user-attachments/assets/1a9ad66d-19be-4d34-bf33-b411a7c03dc8)

# AmberGL API
An OpenGL-inspired API that encapsulates its own Rendering State and manages internal buffers to provide a streamlined interface for Drawing, handling Buffers, and processing Shaders.

## Setting Up the Renderer
Initialize the AmberGL rendering system and configure the basic rendering state:

```cpp
// Initialize the renderer with width and height
AmberGL::Initialize(800, 600);

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

// Create and set up a shader
Resources::Shader* shaderResource = Resources::Loaders::ShaderLoader::Create<Rendering::SoftwareRenderer::Programs::StandardShader>("StandardShader");

// Main rendering loop
while (running) {
    // Clear the screen
    AmberGL::Clear(AGL_COLOR_BUFFER_BIT | AGL_DEPTH_BUFFER_BIT);
    
    // Set shader uniforms
    shaderResource->SetUniform("u_Model", glm::mat4(1.0f));
    shaderResource->SetUniform("u_View", camera.GetViewMatrix());
    shaderResource->SetUniform("u_Projection", camera.GetProjectionMatrix());
    
    // Bind the shader and VAO
    shaderResource->Bind();
    AmberGL::BindVertexArray(VAO);
    
    // Draw the triangle
    AmberGL::DrawArrays(AGL_TRIANGLES, 0, 3);
    
    // Unbind
    AmberGL::BindVertexArray(0);
    shaderResource->Unbind();
    
    // Present the frame
    // ...
}

// Cleanup
Resources::Loaders::ShaderLoader::Destroy(shaderResource);
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

// During rendering:
shaderResource->Bind();
AmberGL::BindVertexArray(VAO);
AmberGL::DrawElements(AGL_TRIANGLES, indices.size());
AmberGL::BindVertexArray(0);
shaderResource->Unbind();
```

### Textured Cube
Create a textured 3D cube:

```cpp
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
// Assuming loadTextureData() returns image data, width, height
uint8_t* data;
uint32_t width, height;
loadTextureData("texture.png", data, width, height); 

AmberGL::TexImage2D(AGL_TEXTURE_2D, 0, AGL_RGBA8, width, height, 0, AGL_RGBA8, AGL_UNSIGNED_BYTE, data);
AmberGL::GenerateMipmap(AGL_TEXTURE_2D);

// Create shader and set uniforms
Resources::Shader* shaderResource = Resources::Loaders::ShaderLoader::Create<Rendering::SoftwareRenderer::Programs::StandardShader>("StandardShader");

// Set up vertex and index data for cube
// ... (define vertices with positions, texture coordinates, and normals)
// ... (define indices for 12 triangles of the cube)

// Main rendering loop
float angle = 0.0f;
while (running) {
    AmberGL::Clear(AGL_COLOR_BUFFER_BIT | AGL_DEPTH_BUFFER_BIT);
    
    // Update model matrix to rotate the cube
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.5f, 1.0f, 0.0f));
    angle += 0.01f;
    
    // Set shader uniforms
    shaderResource->SetUniform("u_Model", model);
    shaderResource->SetUniform("u_View", camera.GetViewMatrix());
    shaderResource->SetUniform("u_Projection", camera.GetProjectionMatrix());
    shaderResource->SetUniform("u_DiffuseMap", 0); // Texture unit 0
    
    // Bind texture
    AmberGL::ActiveTexture(AGL_TEXTURE0);
    AmberGL::BindTexture(AGL_TEXTURE_2D, texture);
    
    // Enable multisampling for smoother edges
    AmberGL::Enable(AGL_MULTISAMPLE);
    
    // Draw the cube
    shaderResource->Bind();
    AmberGL::BindVertexArray(VAO);
    AmberGL::DrawElements(AGL_TRIANGLES, 36); // 36 indices for a cube (6 faces * 2 triangles * 3 vertices)
    AmberGL::BindVertexArray(0);
    shaderResource->Unbind();
}
```

### Framebuffer Operations
Creating and using framebuffers for off-screen rendering:

```cpp
// Create a framebuffer object
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

// Create a texture for the depth attachment
uint32_t depthTexture;
AmberGL::GenTextures(1, &depthTexture);
AmberGL::BindTexture(AGL_TEXTURE_2D, depthTexture);
AmberGL::TexImage2D(AGL_TEXTURE_2D, 0, AGL_DEPTH_COMPONENT, 1024, 1024, 0, AGL_DEPTH_COMPONENT, AGL_FLOAT, nullptr);
AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MIN_FILTER, AGL_NEAREST);
AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MAG_FILTER, AGL_NEAREST);
AmberGL::FramebufferTexture2D(AGL_FRAMEBUFFER, AGL_DEPTH_ATTACHMENT, AGL_TEXTURE_2D, depthTexture, 0);

// Unbind the framebuffer
AmberGL::BindFramebuffer(AGL_FRAMEBUFFER, 0);

// Render to the framebuffer
AmberGL::BindFramebuffer(AGL_FRAMEBUFFER, FBO);
AmberGL::Viewport(0, 0, 1024, 1024);
AmberGL::Clear(AGL_COLOR_BUFFER_BIT | AGL_DEPTH_BUFFER_BIT);

// Draw your scene
// ...

// Switch back to the default framebuffer and render a quad with the framebuffer texture
AmberGL::BindFramebuffer(AGL_FRAMEBUFFER, 0);
AmberGL::Viewport(0, 0, 800, 600);
AmberGL::Clear(AGL_COLOR_BUFFER_BIT | AGL_DEPTH_BUFFER_BIT);

// Create a shader for rendering the framebuffer texture to a quad
Resources::Shader* quadShader = Resources::Loaders::ShaderLoader::Create<Rendering::SoftwareRenderer::Programs::QuadNDC>("QuadShader");
quadShader->SetUniform("u_DepthMap", 0);

// Render a quad with the framebuffer texture
quadShader->Bind();
AmberGL::ActiveTexture(AGL_TEXTURE0);
AmberGL::BindTexture(AGL_TEXTURE_2D, colorTexture); // Or depthTexture for depth visualization
AmberGL::BindVertexArray(quadVAO);
AmberGL::DrawArrays(AGL_TRIANGLE_STRIP, 0, 4);
AmberGL::BindVertexArray(0);
quadShader->Unbind();
```

### Multisampling
Enabling multisampling for anti-aliasing:

```cpp
// Configure multisampling during initialization
AmberGL::WindowHint(AGL_SAMPLES, 4); // 4x multisampling
AmberGL::Enable(AGL_MULTISAMPLE);    // Enable multisampling

// Rest of the rendering process remains the same
// AmberGL will automatically perform multisampling during rasterization

// To disable multisampling for specific parts of rendering
AmberGL::Disable(AGL_MULTISAMPLE);
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
