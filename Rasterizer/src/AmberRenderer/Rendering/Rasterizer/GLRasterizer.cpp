#include "AmberRenderer/Rendering/Rasterizer/GLRasterizer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <algorithm>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>

#include "AmberRenderer/Rendering/Rasterizer/Buffers/MSAABuffer.h"
#include "AmberRenderer/Geometry/Plane.h"
#include "AmberRenderer/Geometry/Polygon.h"
#include "AmberRenderer/Geometry/Triangle.h"

typedef AmberRenderer::Data::Color RGBA8;
typedef float Depth;

void InitializeClippingFrustum();

void RasterizeTriangle(uint8_t p_primitiveMode, const AmberRenderer::Geometry::Vertex& p_vertex0, const AmberRenderer::Geometry::Vertex& p_vertex1, const AmberRenderer::Geometry::Vertex& p_vertex2);
void RasterizeLine(const AmberRenderer::Geometry::Vertex& p_vertex0, const AmberRenderer::Geometry::Vertex& p_vertex1, const AmberRenderer::Data::Color& p_color);
void TransformAndRasterizeVertices(const uint8_t p_primitiveMode, const std::array<glm::vec4, 3>& processedVertices);
void ComputeFragments(const AmberRenderer::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices);

void SetFragment(const AmberRenderer::Geometry::Triangle& p_triangle, uint32_t p_x, uint32_t p_y, const std::array<glm::vec4, 3>& p_transformedVertices);
void SetSampleFragment(const AmberRenderer::Geometry::Triangle& p_triangle, uint32_t p_x, uint32_t p_y, float p_sampleX, float p_sampleY, uint8_t p_sampleIndex, const std::array<glm::vec4, 3>& p_transformedVertices);

void RasterizeTriangleWireframe(const AmberRenderer::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices);
void RasterizeLine(const AmberRenderer::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices, const glm::vec4& p_start, const glm::vec4& p_end);
void RasterizeLine(const glm::vec4& p_start, const glm::vec4& p_end, const AmberRenderer::Data::Color& p_color);

void RasterizeTrianglePoints(const AmberRenderer::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices);
void DrawPoint(const AmberRenderer::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices, const glm::vec4& p_point);
void DrawPoint(const glm::vec2& p_point, const AmberRenderer::Data::Color& p_color);

glm::vec3 ComputeScreenSpaceCoordinate(const glm::vec4& p_vertexWorldPosition);
glm::vec2 ComputeNormalizedDeviceCoordinate(const glm::vec3& p_vertexScreenSpacePosition);
glm::vec2 ComputeRasterSpaceCoordinate(glm::vec2 p_vertexNormalizedPosition);

void ClipAgainstPlane(AmberRenderer::Geometry::Polygon& p_polygon, const AmberRenderer::Geometry::Plane& p_plane);
void ApplyMSAA();

//TODO: encapsulate in utils.
inline void SSEClear(uint32_t* p_data, size_t p_size, uint32_t p_value)
{
	__m128i fillValue = _mm_set1_epi32(static_cast<int>(p_value));

	size_t blockCount = p_size / 4;

	for (size_t i = 0; i < blockCount; ++i)
	{
		_mm_storeu_si128(reinterpret_cast<__m128i*>(p_data + i * 4), fillValue);
	}

	for (size_t i = blockCount * 4; i < p_size; ++i)
	{
		p_data[i] = p_value;
	}
}

static uint32_t floatToUint32(float p_value)
{
	uint32_t bits;
	std::memcpy(&bits, &p_value, sizeof(float));
	return bits;
}

static float uint32ToFloat(uint32_t p_bits)
{
	float value;
	std::memcpy(&value, &p_bits, sizeof(float));
	return value;
}

namespace
{
	struct RenderContext
	{
		uint16_t ViewPortX = 0;
		uint16_t ViewPortY = 0;
		uint16_t ViewPortWidth = 0;
		uint16_t ViewPortHeight = 0;
		uint8_t State = 0;
		uint8_t PolygonMode = GLR_FILL;
		uint8_t CullFace = GLR_BACK;
		uint8_t Samples = 0;
		AmberRenderer::Rendering::Rasterizer::Shaders::AShader* Shader = nullptr;
	};

	struct VertexArrayObject
	{
		uint32_t ID                 = 0;
		uint32_t BoundArrayBuffer   = 0;
		uint32_t BoundElementBuffer = 0;
	};

	struct BufferObject
	{
		uint32_t ID     = 0;
		uint32_t Target = 0;
		size_t Size     = 0;
		std::vector<uint8_t> Data;
	};

	//TODO: Implement render buffer
	template<typename T>
	struct FrameBufferObjectData
	{
		static_assert(std::is_same_v<T, RGBA8> || std::is_same_v<T, Depth>, "FrameBuffer only supports RGBA8 (Color) or Depth (float).");

		uint32_t Width;
		uint32_t Height;
		uint32_t Size;
		uint32_t RowSize;
		uint32_t SizeInByte;
		uint32_t* Data;
		uint32_t ClearColor;

		FrameBufferObjectData(uint32_t p_width, uint32_t p_height) :
			Width(p_width),
			Height(p_height),
			Size(Width* Height),
			RowSize(Width * sizeof(uint32_t)),
			SizeInByte(Size * sizeof(uint32_t)),
			Data(new uint32_t[Size]), ClearColor(0)
		{
		}

		~FrameBufferObjectData()
		{
			delete[] Data;
			Data = nullptr;
		}

		void SetColor(float p_red, float p_green, float p_blue, float p_alpha)
		{
			uint8_t r = static_cast<uint8_t>(std::round(std::clamp(p_red, 0.0f, 1.0f) * 255.0f));
			uint8_t g = static_cast<uint8_t>(std::round(std::clamp(p_green, 0.0f, 1.0f) * 255.0f));
			uint8_t b = static_cast<uint8_t>(std::round(std::clamp(p_blue, 0.0f, 1.0f) * 255.0f));
			uint8_t a = static_cast<uint8_t>(std::round(std::clamp(p_alpha, 0.0f, 1.0f) * 255.0f));

			ClearColor = (r << 24) | (g << 16) | (b << 8) | a;
		}

		void Clear() const
		{
			if constexpr (std::is_same<T, RGBA8>::value)
			{
				SSEClear(Data, Size, ClearColor);
			}
			else if constexpr (std::is_same<T, Depth>::value)
			{
				uint32_t depthVal = floatToUint32(std::numeric_limits<float>::max());
				SSEClear(Data, Size, depthVal);
			}
		}

		void Resize(uint32_t p_width, uint32_t p_height)
		{
			if (Width == p_width && Height == p_height)
				return;

			delete[] Data;

			Width = p_width;
			Height = p_height;
			Size = Width * Height;
			RowSize = Width * sizeof(uint32_t);

			Data = new uint32_t[Size];
			Clear();
		}

		void SetPixel(int p_x, int p_y, const T& p_value) const
		{
			if constexpr (std::is_same<T, RGBA8>::value)
			{
				Data[p_y * Width + p_x] = p_value.Pack();
			}
			else if constexpr (std::is_same<T, Depth>::value)
			{
				Data[p_y * Width + p_x] = floatToUint32(p_value);
			}
		}

		T GetPixel(uint32_t p_x, uint32_t p_y) const
		{
			if constexpr (std::is_same<T, RGBA8>::value)
			{
				return Data[p_y * Width + p_x];
			}
			else if constexpr (std::is_same<T, Depth>::value)
			{
				return uint32ToFloat(Data[p_y * Width + p_x]);
			}

			return T{};
		}

		void UpdateTexture(TextureObject* p_texture) const
		{
			if (!p_texture)
				return;

			uint32_t bytes = Size * 4;
			uint8_t* textureData = new uint8_t[bytes];

			if constexpr (std::is_same<T, Depth>::value)
			{
				for (uint32_t y = 0; y < Height; y++)
				{
					for (uint32_t x = 0; x < Width; x++)
					{
						uint32_t flippedY = Height - 1 - y;
						uint32_t index = flippedY * Width + x;

						float depth = uint32ToFloat(Data[y * Width + x]);
						depth = std::clamp(depth, 0.0f, 1.0f);
						uint8_t d = static_cast<uint8_t>(depth * 255.0f);

						textureData[index * 4 + 0] = d;
						textureData[index * 4 + 1] = 0;
						textureData[index * 4 + 2] = 0;
						textureData[index * 4 + 3] = 255;
					}
				}
			}
			if constexpr (std::is_same<T, RGBA8>::value)
			{
				for (uint32_t y = 0; y < Height; y++)
				{
					for (uint32_t x = 0; x < Width; x++)
					{
						uint32_t flippedY = Height - 1 - y;
						uint32_t index = flippedY * Width + x;

						AmberRenderer::Data::Color color(Data[y * Width + x]);

						textureData[index * 4 + 0] = color.r;
						textureData[index * 4 + 1] = color.g;
						textureData[index * 4 + 2] = color.b;
						textureData[index * 4 + 3] = color.a;
					}
				}
			}

			std::memcpy(p_texture->Data8, textureData, bytes);
			delete[] textureData;
		}
	};

	struct FrameBufferObject
	{
		uint32_t ID = 0;
		FrameBufferObjectData<RGBA8>* ColorBuffer = nullptr;
		FrameBufferObjectData<Depth>* DepthBuffer = nullptr;
		bool ColorWriteEnabled = true;
		TextureObject* AttachedTexture = nullptr;

		~FrameBufferObject()
		{
			delete ColorBuffer;
			delete DepthBuffer;
		}
	};

	RenderContext RenderContext;

	AmberRenderer::Rendering::Rasterizer::Buffers::MSAABuffer* MSAABuffer            = nullptr;

	std::array<AmberRenderer::Geometry::Plane, 6> ClippingFrustum;

	std::unordered_map<uint32_t, VertexArrayObject> VertexArrayObjects;
	std::unordered_map<uint32_t, BufferObject> BufferObjects;

	uint32_t VertexArrayID = 1;
	uint32_t CurrentVertexArrayObject = 0;

	uint32_t BufferID = 1;
	uint32_t CurrentArrayBuffer = 0;
	uint32_t CurrentElementBuffer = 0;

	VertexArrayObject* GetBoundVertexArrayObject()
	{
		if (CurrentVertexArrayObject == 0)
			return nullptr;

		auto it = VertexArrayObjects.find(CurrentVertexArrayObject);

		return (it != VertexArrayObjects.end()) ? &it->second : nullptr;
	}

	std::unordered_map<uint32_t, TextureObject*> TextureObjects;
	uint32_t TextureID = 1;
	uint32_t CurrentTexture = 0;

	constexpr uint32_t MAX_TEXTURE_UNITS = 16;
	uint32_t CurrentActiveTextureUnit = 0;
	TextureObject* BoundTextureUnits[MAX_TEXTURE_UNITS] = { nullptr };

	FrameBufferObjectData<RGBA8>* FrameBuffer = nullptr;
	FrameBufferObjectData<Depth>* DepthBuffer = nullptr;
	FrameBufferObjectData<RGBA8>* ActiveFrameBuffer = nullptr;
	FrameBufferObjectData<Depth>* ActiveDepthBuffer = nullptr;

	std::unordered_map<uint32_t, FrameBufferObject> FrameBufferObjects;
	uint32_t FrameBufferObjectID = 1;
	uint32_t CurrentFrameBuffer = 0;
}

void GLRasterizer::GenVertexArrays(uint32_t p_count, uint32_t* p_arrays)
{
	for (uint32_t i = 0; i < p_count; i++) 
	{
		VertexArrayObject vertexArrayObject;
		vertexArrayObject.ID = VertexArrayID;
		vertexArrayObject.BoundArrayBuffer = 0;
		vertexArrayObject.BoundElementBuffer = 0;
		VertexArrayObjects[VertexArrayID] = vertexArrayObject;
		p_arrays[i] = VertexArrayID;
		VertexArrayID++;
	}
}

void GLRasterizer::DeleteVertexArrays(uint32_t p_count, const uint32_t* p_arrays)
{
	for (uint32_t i = 0; i < p_count; i++) 
	{
		uint32_t id = p_arrays[i];

		VertexArrayObjects.erase(id);

		if (CurrentVertexArrayObject == id)
			CurrentVertexArrayObject = 0;
	}
}

void GLRasterizer::BindVertexArray(uint32_t p_array)
{
	if (p_array != 0 && VertexArrayObjects.find(p_array) == VertexArrayObjects.end()) 
	{
		std::cout << "VertexArray with ID " << p_array << " not found!\n";
		return;
	}

	CurrentVertexArrayObject = p_array;
}

void GLRasterizer::GenBuffers(uint32_t p_count, uint32_t* p_buffers)
{
	for (uint32_t i = 0; i < p_count; i++) 
	{
		BufferObject bufferObject;
		bufferObject.ID = BufferID;
		bufferObject.Target = 0;
		bufferObject.Size = 0;
		bufferObject.Data.clear();
		BufferObjects[BufferID] = bufferObject;
		p_buffers[i] = BufferID;

		BufferID++;
	}
}

void GLRasterizer::DeleteBuffers(uint32_t p_count, const uint32_t* p_buffers)
{
	for (uint32_t i = 0; i < p_count; i++) 
	{
		uint32_t id = p_buffers[i];

		BufferObjects.erase(id);

		if (CurrentArrayBuffer == id)
			CurrentArrayBuffer = 0;

		if (CurrentElementBuffer == id)
			CurrentElementBuffer = 0;
	}
}

void GLRasterizer::BindBuffer(uint32_t p_target, uint32_t p_buffer)
{
	if (p_target == GLR_ARRAY_BUFFER) 
	{
		CurrentArrayBuffer = p_buffer;

		if (CurrentVertexArrayObject != 0)
			VertexArrayObjects[CurrentVertexArrayObject].BoundArrayBuffer = p_buffer;
	}
	else if (p_target == GLR_ELEMENT_ARRAY_BUFFER)
	{
		CurrentElementBuffer = p_buffer;

		if (CurrentVertexArrayObject != 0)
			VertexArrayObjects[CurrentVertexArrayObject].BoundElementBuffer = p_buffer;
	}
}

void GLRasterizer::BufferData(uint32_t p_target, size_t p_size, const void* p_data)
{
	uint32_t currentBuffer = (p_target == GLR_ARRAY_BUFFER) ? CurrentArrayBuffer : (p_target == GLR_ELEMENT_ARRAY_BUFFER) ? CurrentElementBuffer : 0;

	if (currentBuffer == 0) 
	{
		std::cout << "No buffer bound for target " << p_target << "\n";
		return;
	}

	BufferObject& bufferObject = BufferObjects[currentBuffer];
	bufferObject.Target = p_target;
	bufferObject.Size = p_size;
	bufferObject.Data.resize(p_size);
	std::memcpy(bufferObject.Data.data(), p_data, p_size);
}

void GLRasterizer::GenTextures(uint32_t p_count, uint32_t* p_textures)
{
	for (uint32_t i = 0; i < p_count; i++) 
	{
		TextureObject* texture = new TextureObject();
		texture->ID = TextureID;
		p_textures[i] = TextureID;
		TextureObjects[TextureID] = texture;
		TextureID++;
	}
}

void GLRasterizer::DeleteTextures(uint32_t p_count, const uint32_t* p_textures)
{
	for (uint32_t i = 0; i < p_count; i++) 
	{
		uint32_t id = p_textures[i];

		auto it = TextureObjects.find(id);
		if (it != TextureObjects.end()) 
		{
			delete it->second;
			TextureObjects.erase(it);
		}

		if (CurrentTexture == id)
			CurrentTexture = 0;
	}
}

void GLRasterizer::BindTexture(uint32_t p_target, uint32_t p_texture)
{
	if (p_target != GLR_TEXTURE_2D) 
	{
		std::cout << "BindTexture: Only GLR_TEXTURE_2D supported.\n";
		return;
	}
	if (p_texture != 0 && TextureObjects.find(p_texture) == TextureObjects.end()) 
	{
		std::cout << "BindTexture: Texture with ID " << p_texture << " not found.\n";
		return;
	}
	CurrentTexture = p_texture;

	if (p_texture == 0)
		BoundTextureUnits[CurrentActiveTextureUnit] = nullptr;
	else
		BoundTextureUnits[CurrentActiveTextureUnit] = TextureObjects[CurrentTexture];
}

void GLRasterizer::TexImage2D(uint32_t p_target, uint32_t p_level, uint32_t p_internalFormat, uint32_t p_width, uint32_t p_height, uint32_t p_border, uint32_t p_format, uint32_t p_type, const void* p_data)
{
	if (p_target != GLR_TEXTURE_2D) 
	{
		std::cout << "TexImage2D: Only GLR_TEXTURE_2D supported.\n";
		return;
	}

	if (CurrentTexture == 0) 
	{
		std::cout << "TexImage2D: No texture bound.\n";
		return;
	}

	TextureObject& texture = *TextureObjects[CurrentTexture];
	texture.InternalFormat = p_internalFormat;
	texture.Width = p_width;
	texture.Height = p_height;
	texture.Target = p_target;

	if (p_internalFormat == GLR_RGBA8 && p_type == GLR_UNSIGNED_BYTE)
	{
		if (texture.Data8)
		{
			delete[] texture.Data8;
			texture.Data8 = nullptr;
		}

		texture.Data8 = new uint8_t[p_width * p_height * 4];

		if (p_data) 
		{
			std::memcpy(texture.Data8, p_data, p_width * p_height * 4);
		}
		else
		{
			std::fill(texture.Data8, texture.Data8 + p_width * p_height * 4, 0);
		}
	}
	else if (p_internalFormat == GLR_DEPTH_COMPONENT && p_type == GLR_FLOAT) 
	{
		//TODO: Maybe use 32 bit buffer and 8 bit. splitting depth value.
		if (texture.Data8)
		{
			delete[] texture.Data8;
			texture.Data8 = nullptr;
		}

		texture.Data8 = new uint8_t[p_width * p_height * 4];

		if (p_data) 
		{
			std::memcpy(texture.Data8, p_data, p_width * p_height * 4);
		}
		else 
		{
			std::fill(texture.Data8, texture.Data8 + p_width * p_height * 4, 0);
		}

		/*// Also allocate a Data8 buffer for display (we store depth in R channel)
		if (texture.Data8)
		{
			delete[] texture.Data8; texture.Data8 = nullptr;
		}

		texture.Data8 = new uint8_t[p_width * p_height * 4];*/
	}
	else 
	{
		std::cout << "TexImage2D: Unsupported internalFormat/type combination.\n";
	}
}

void GLRasterizer::TexParameteri(uint32_t p_target, uint32_t p_pname, uint8_t p_param)
{
	if (p_target != GLR_TEXTURE_2D) 
	{
		std::cout << "TexParameteri: Only GLR_TEXTURE_2D supported.\n";
		return;
	}

	if (CurrentTexture == 0) 
	{
		std::cout << "TexParameteri: No texture bound.\n";
		return;
	}

	TextureObject& texture = *TextureObjects[CurrentTexture];

	switch (p_pname)
	{
	case GLR_TEXTURE_MIN_FILTER:
		texture.MinFilter = p_param;
		break;
	case GLR_TEXTURE_MAG_FILTER:
		texture.MagFilter = p_param;
		break;
	case GLR_TEXTURE_WRAP_S:
		texture.WrapS = p_param;
		break;
	case GLR_TEXTURE_WRAP_T:
		texture.WrapT = p_param;
		break;
	default:
		std::cout << "TexParameteri: Unsupported pname " << p_param << "\n";
		break;
	}
}

void GLRasterizer::ActiveTexture(uint32_t p_unit)
{
	if (p_unit >= MAX_TEXTURE_UNITS)
	{
		std::cout << "ActiveTexture: Texture unit " << p_unit << " is out of range (max " << MAX_TEXTURE_UNITS - 1 << ").\n";
		return;
	}

	CurrentActiveTextureUnit = p_unit;
}

void GLRasterizer::GenerateMipmap(uint32_t p_target)
{
	if (p_target != GLR_TEXTURE_2D)
	{
		std::cout << "BindTexture: Only GLR_TEXTURE_2D supported.\n";
		return;
	}

	TextureObject& textureObject = *TextureObjects[CurrentTexture];


	int width = textureObject.Width;
	int height = textureObject.Height;

	int maxLevel = 1 + static_cast<int>(std::floor(std::log2(std::max(width, height))));

	textureObject.Mipmaps = new uint8_t * [maxLevel];

	textureObject.Mipmaps[0] = new uint8_t[width * height * 4];
	std::memcpy(textureObject.Mipmaps[0], textureObject.Data8, width * height * 4);

	for (int level = 1; level < maxLevel; level++)
	{
		int prevWidth = width;
		int prevHeight = height;

		width = std::max(1, width / 2);
		height = std::max(1, height / 2);

		textureObject.Mipmaps[level] = new uint8_t[width * height * 4];

		uint8_t* previousData = textureObject.Mipmaps[level - 1];
		uint8_t* currentData = textureObject.Mipmaps[level];

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int x2 = x * 2;
				int y2 = y * 2;

				int x2End = std::min(x2 + 2, prevWidth);
				int y2End = std::min(y2 + 2, prevHeight);

				int pixelCount = (x2End - x2) * (y2End - y2);

				float sum[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

				for (int sy = y2; sy < y2End; sy++)
				{
					for (int sx = x2; sx < x2End; sx++)
					{
						int idx = (sy * prevWidth + sx) * 4;
						for (int c = 0; c < 4; c++)
						{
							sum[c] += previousData[idx + c];
						}
					}
				}

				int dstIdx = (y * width + x) * 4;
				for (int c = 0; c < 4; c++)
				{
					currentData[dstIdx + c] = static_cast<uint8_t>(sum[c] / pixelCount);
				}
			}
		}
	}
}

TextureObject* GLRasterizer::GetTextureObject(uint32_t p_textureUnit)
{
	if (p_textureUnit >= MAX_TEXTURE_UNITS)
	{
		std::cout << "GetTextureObject: Texture unit " << p_textureUnit << " is out of range.\n";
		return nullptr;
	}
	return BoundTextureUnits[p_textureUnit];
}

void GLRasterizer::GenFramebuffers(uint32_t p_count, uint32_t* p_framebuffers)
{
	for (uint32_t i = 0; i < p_count; i++)
	{
		FrameBufferObject frameBufferObject;
		frameBufferObject.ID = FrameBufferObjectID;
		FrameBufferObjects[FrameBufferObjectID] = frameBufferObject;
		p_framebuffers[i] = FrameBufferObjectID;
		FrameBufferObjectID++;
	}
}

void GLRasterizer::BindFramebuffer(uint32_t p_target, uint32_t p_framebuffer)
{
	if (p_target != GLR_FRAMEBUFFER)
	{
		std::cout << "BindFramebuffer: only GLR_FRAMEBUFFER is supported.\n";
		return;
	}

	if (p_framebuffer != 0 && FrameBufferObjects.find(p_framebuffer) == FrameBufferObjects.end())
	{
		std::cout << "Framebuffer " << p_framebuffer << " not found!\n";
		return;
	}

	if (p_framebuffer == 0)
	{
		if (CurrentFrameBuffer != 0)
		{
			FrameBufferObject& frameBufferObject = FrameBufferObjects[CurrentFrameBuffer];

			if (frameBufferObject.DepthBuffer && frameBufferObject.AttachedTexture)
			{
				//TODO: not the good place to update the attached texture
				frameBufferObject.DepthBuffer->UpdateTexture(frameBufferObject.AttachedTexture);
			}
		}

		ActiveDepthBuffer = DepthBuffer;
		ActiveFrameBuffer = FrameBuffer;
		CurrentFrameBuffer = 0;
	}
	else
	{
		CurrentFrameBuffer = p_framebuffer;
		FrameBufferObject& frameBufferObject = FrameBufferObjects[CurrentFrameBuffer];

		ActiveDepthBuffer = frameBufferObject.DepthBuffer;
		ActiveFrameBuffer = frameBufferObject.ColorWriteEnabled ? frameBufferObject.ColorBuffer : nullptr;
	}
}

void GLRasterizer::FramebufferTexture2D(uint32_t p_target, uint32_t p_attachment, uint32_t p_textarget, uint32_t p_texture, int p_level)
{
	if (p_target != GLR_FRAMEBUFFER)
	{
		std::cout << "FramebufferTexture2D: only GLR_FRAMEBUFFER is supported.\n";
		return;
	}

	if (CurrentFrameBuffer == 0)
	{
		std::cout << "No FBO currently bound.\n";
		return;
	}

	if (p_texture != 0 && TextureObjects.find(p_texture) == TextureObjects.end())
	{
		std::cout << "Texture " << p_texture << " not found.\n";
		return;
	}

	FrameBufferObject& frameBufferObject = FrameBufferObjects[CurrentFrameBuffer];

	if (p_texture == 0)
	{
		if (p_attachment == GLR_DEPTH_ATTACHMENT)
		{
			delete frameBufferObject.DepthBuffer;
			frameBufferObject.DepthBuffer = nullptr;
			
			if (frameBufferObject.AttachedTexture && p_attachment == GLR_DEPTH_ATTACHMENT)
				frameBufferObject.AttachedTexture = nullptr;
		}
		else if (p_attachment == GLR_COLOR_ATTACHMENT)
		{
			delete frameBufferObject.ColorBuffer;
			frameBufferObject.ColorBuffer = nullptr;
		}
		return;
	}

	TextureObject* textureObject = TextureObjects[p_texture];
	
	if (p_attachment == GLR_DEPTH_ATTACHMENT)
	{
		delete frameBufferObject.DepthBuffer;

		frameBufferObject.DepthBuffer = new FrameBufferObjectData<Depth>(textureObject->Width, textureObject->Height);
		frameBufferObject.AttachedTexture = textureObject;
	}
	else if (p_attachment == GLR_COLOR_ATTACHMENT)
	{
		delete frameBufferObject.ColorBuffer;

		frameBufferObject.ColorBuffer = new FrameBufferObjectData<RGBA8>(textureObject->Width, textureObject->Height);
	}
}

void GLRasterizer::DrawBuffer(uint32_t p_mode)
{
	if (CurrentFrameBuffer == 0)
		return;

	FrameBufferObject& frameBufferObject = FrameBufferObjects[CurrentFrameBuffer];

	if (p_mode == GL_NONE)
	{
		frameBufferObject.ColorWriteEnabled = false;
	}
	else
	{
		frameBufferObject.ColorWriteEnabled = true;
	}
}

void GLRasterizer::ReadBuffer(uint32_t p_mode)
{
	//TODO
}

uint32_t* GLRasterizer::GetFrameBufferData()
{
	return ActiveFrameBuffer->Data;
}

uint32_t GLRasterizer::GetFrameBufferRowSize()
{
	return ActiveFrameBuffer->RowSize;
}

void GLRasterizer::Initialize(uint16_t p_rasterizationBufferWidth, uint16_t p_rasterizationBufferHeight)
{
	RenderContext.ViewPortWidth = p_rasterizationBufferWidth;
	RenderContext.ViewPortHeight = p_rasterizationBufferHeight;

	FrameBuffer = new FrameBufferObjectData<RGBA8>(p_rasterizationBufferWidth, p_rasterizationBufferHeight);
	DepthBuffer = new FrameBufferObjectData<Depth>(p_rasterizationBufferWidth, p_rasterizationBufferHeight);

	ActiveFrameBuffer = FrameBuffer;
	ActiveDepthBuffer = DepthBuffer;

	MSAABuffer = new AmberRenderer::Rendering::Rasterizer::Buffers::MSAABuffer(p_rasterizationBufferWidth, p_rasterizationBufferHeight);

	InitializeClippingFrustum();
}

void GLRasterizer::DrawElements(uint8_t p_primitiveMode, uint32_t p_indexCount)
{
	FrameBufferObjectData<RGBA8>* originalColorBuffer = ActiveFrameBuffer;
	FrameBufferObjectData<Depth>* originalDepthBuffer = ActiveDepthBuffer;

	if (CurrentFrameBuffer != 0)
	{
		FrameBufferObject& fbo = FrameBufferObjects[CurrentFrameBuffer];
		ActiveDepthBuffer = fbo.DepthBuffer;
		ActiveFrameBuffer = fbo.ColorWriteEnabled ? fbo.ColorBuffer : nullptr;
	}
	else
	{
		ActiveFrameBuffer = FrameBuffer;
		ActiveDepthBuffer = DepthBuffer;
	}

	VertexArrayObject* vao = GetBoundVertexArrayObject();
	if (!vao || vao->BoundElementBuffer == 0 || vao->BoundArrayBuffer == 0)
	{
		std::cout << "Invalid VAO or buffers for drawing!\n";

		ActiveFrameBuffer = originalColorBuffer;
		ActiveDepthBuffer = originalDepthBuffer;
		return;
	}

	auto itIndex = BufferObjects.find(vao->BoundElementBuffer);
	auto itVertex = BufferObjects.find(vao->BoundArrayBuffer);
	if (itIndex == BufferObjects.end() || itVertex == BufferObjects.end())
	{
		std::cout << "Buffers not found!\n";
		
		ActiveFrameBuffer = originalColorBuffer;
		ActiveDepthBuffer = originalDepthBuffer;
		return;
	}

	
	BufferObject& indexBufferObject = itIndex->second;
	BufferObject& vertexBuffer = itVertex->second;
	AmberRenderer::Geometry::Vertex* vertices = reinterpret_cast<AmberRenderer::Geometry::Vertex*>(vertexBuffer.Data.data());
	uint32_t* indices = reinterpret_cast<uint32_t*>(indexBufferObject.Data.data());

	
	size_t availableIndices = indexBufferObject.Size / sizeof(uint32_t);
	uint32_t actualCount = std::min(p_indexCount, static_cast<uint32_t>(availableIndices));
	for (size_t i = 0; i + 2 < actualCount; i += 3)
	{
		RasterizeTriangle(p_primitiveMode, vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
	}

	
	if (RenderContext.State & GLR_MULTISAMPLE && ActiveFrameBuffer != nullptr)
		ApplyMSAA();

	ActiveFrameBuffer = originalColorBuffer;
	ActiveDepthBuffer = originalDepthBuffer;
}

void GLRasterizer::DrawArrays(uint8_t p_primitiveMode, uint32_t p_first, uint32_t p_count)
{
	FrameBufferObjectData<RGBA8>* originalColorBuffer = ActiveFrameBuffer;
	FrameBufferObjectData<Depth>* originalDepthBuffer = ActiveDepthBuffer;

	if (CurrentFrameBuffer != 0)
	{
		FrameBufferObject& fbo = FrameBufferObjects[CurrentFrameBuffer];
		ActiveDepthBuffer = fbo.DepthBuffer;
		ActiveFrameBuffer = fbo.ColorWriteEnabled ? fbo.ColorBuffer : nullptr;
	}
	else
	{
		ActiveFrameBuffer = FrameBuffer;
		ActiveDepthBuffer = DepthBuffer;
	}

	VertexArrayObject* vao = GetBoundVertexArrayObject();
	if (!vao || vao->BoundArrayBuffer == 0)
	{
		std::cout << "Invalid VAO or buffers for drawing!\n";

		ActiveFrameBuffer = originalColorBuffer;
		ActiveDepthBuffer = originalDepthBuffer;
		return;
	}

	
	auto itVertex = BufferObjects.find(vao->BoundArrayBuffer);
	if (itVertex == BufferObjects.end())
	{
		std::cout << "Vertex buffer not found!\n";

		ActiveFrameBuffer = originalColorBuffer;
		ActiveDepthBuffer = originalDepthBuffer;
		return;
	}

	BufferObject& vertexBuffer = itVertex->second;
	AmberRenderer::Geometry::Vertex* vertices = reinterpret_cast<AmberRenderer::Geometry::Vertex*>(vertexBuffer.Data.data());

	if (p_primitiveMode == GLR_TRIANGLE_STRIP)
	{
		for (uint32_t i = p_first; i < p_first + p_count - 2; i++)
		{
			if ((i - p_first) % 2 == 0)
			{
				RasterizeTriangle(p_primitiveMode, vertices[i], vertices[i + 1], vertices[i + 2]);
			}
			else
			{
				RasterizeTriangle(p_primitiveMode, vertices[i + 1], vertices[i], vertices[i + 2]);
			}
		}
	}
	else if (p_primitiveMode == GLR_TRIANGLES)
	{
		for (uint32_t i = p_first; i + 2 < p_first + p_count; i += 3)
		{
			RasterizeTriangle(p_primitiveMode, vertices[i], vertices[i + 1], vertices[i + 2]);
		}
	}

	if (RenderContext.State & GLR_MULTISAMPLE && ActiveFrameBuffer != nullptr)
		ApplyMSAA();

	ActiveFrameBuffer = originalColorBuffer;
	ActiveDepthBuffer = originalDepthBuffer;
}

void GLRasterizer::DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, const AmberRenderer::Data::Color& p_color)
{
	RasterizeLine(AmberRenderer::Geometry::Vertex(p_point0 ), AmberRenderer::Geometry::Vertex(p_point1), p_color);
}

void GLRasterizer::UseProgram(AmberRenderer::Rendering::Rasterizer::Shaders::AShader* p_shader)
{
	RenderContext.Shader = p_shader;
}

void GLRasterizer::SetSamples(uint8_t p_samples)
{
	RenderContext.Samples = p_samples;
	MSAABuffer->SetSamplesAmount(p_samples);
}

void GLRasterizer::PolygonMode(uint8_t p_mode)
{
	if (p_mode <= GLR_POINT)
		RenderContext.PolygonMode = p_mode;
	else
		RenderContext.PolygonMode = GLR_FILL;
}

void GLRasterizer::Enable(uint8_t p_state)
{
	RenderContext.State |= p_state;
}

void GLRasterizer::Disable(uint8_t p_state)
{
	RenderContext.State &= ~p_state;
}

bool GLRasterizer::IsEnabled(uint8_t p_capability)
{
	return (RenderContext.State & p_capability) != 0;
}

void GLRasterizer::CullFace(uint8_t p_face)
{
	if (p_face <= GLR_FRONT_AND_BACK)
		RenderContext.CullFace = p_face;
	else
		RenderContext.CullFace = GLR_BACK;
}

void GLRasterizer::DepthMask(bool p_flag)
{
	if (p_flag) 
	{
		Enable(GLR_DEPTH_WRITE);
	}
	else 
	{
		Disable(GLR_DEPTH_WRITE);
	}
}

void GLRasterizer::GetBool(uint8_t p_name, bool* p_params)
{
	if (!p_params) return;

	switch (p_name)
	{
	case GLR_DEPTH_WRITE:
		*p_params = (RenderContext.State & GLR_DEPTH_WRITE) != 0;
		break;
	case GLR_DEPTH_TEST:
		*p_params = (RenderContext.State & GLR_DEPTH_TEST) != 0;
		break;
	case GLR_CULL_FACE:
		*p_params = (RenderContext.State & GLR_CULL_FACE) != 0;
		break;
	default:
		*p_params = false;
		break;
	}
}

void GLRasterizer::GetInt(uint8_t p_name, int* p_params)
{
	if (!p_params) return;

	switch (p_name)
	{
	case GLR_CULL_FACE:
		*p_params = RenderContext.CullFace;
		break;
	case GLR_FILL:
	case GLR_LINE:
	case GLR_POINT:
		*p_params = RenderContext.PolygonMode;
		break;
	case GLR_VIEW_PORT:
		p_params[0] = RenderContext.ViewPortX;
		p_params[1] = RenderContext.ViewPortY;
		p_params[2] = RenderContext.ViewPortWidth;
		p_params[3] = RenderContext.ViewPortHeight;
		break;
	default:
		*p_params = -1;
		break;
	}
}

void GLRasterizer::Terminate()
{
	for (auto& [id, texture] : TextureObjects)
	{
		if (texture)
		{
			if (texture->Mipmaps)
			{
				int maxLevel = 1 + static_cast<int>(std::floor(std::log2(std::max(texture->Width, texture->Height))));

				for (int i = 0; i < maxLevel; i++)
				{
					delete[] texture->Mipmaps[i];
					texture->Mipmaps[i] = nullptr;
				}

				delete[] texture->Mipmaps;
				texture->Mipmaps = nullptr;
			}

			delete[] texture->Data8;
			texture->Data8 = nullptr;

			delete texture;
			texture = nullptr;
		}
	}

	TextureObjects.clear();

	FrameBufferObjects.clear();

	BufferObjects.clear();

	VertexArrayObjects.clear();

	CurrentVertexArrayObject = 0;
	CurrentArrayBuffer = 0;
	CurrentElementBuffer = 0;
	CurrentTexture = 0;
	CurrentFrameBuffer = 0;

	VertexArrayID = 1;
	BufferID = 1;
	TextureID = 1;
	FrameBufferObjectID = 1;

	delete FrameBuffer;
	FrameBuffer = nullptr;
	ActiveFrameBuffer = nullptr;

	delete DepthBuffer;
	DepthBuffer = nullptr;
	ActiveDepthBuffer = nullptr;

	delete MSAABuffer;
	MSAABuffer = nullptr;

	for (uint32_t i = 0; i < MAX_TEXTURE_UNITS; i++)
	{
		BoundTextureUnits[i] = nullptr;
	}
}

void GLRasterizer::WindowHint(uint8_t p_name, uint8_t p_value)
{
	if (p_name == GLR_SAMPLES)
	{
		SetSamples(p_value);
	}
}

void GLRasterizer::ClearColor(float p_red, float p_green, float p_blue, float p_alpha)
{
	if (ActiveFrameBuffer)
		ActiveFrameBuffer->SetColor(p_red, p_green, p_blue, p_alpha);

	if (RenderContext.State & GLR_MULTISAMPLE)
		MSAABuffer->SetColor(p_red, p_green, p_blue, p_alpha);
}

void GLRasterizer::Clear(uint8_t p_flags)
{
	if (p_flags & GLR_COLOR_BUFFER_BIT)
	{
		if (ActiveFrameBuffer)
			ActiveFrameBuffer->Clear();
	}

	if (p_flags & GLR_DEPTH_BUFFER_BIT)
	{
		ActiveDepthBuffer->Clear();
	}

	if (RenderContext.State & GLR_MULTISAMPLE)
		MSAABuffer->Clear();
}

void GLRasterizer::Viewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height)
{
	if (RenderContext.ViewPortWidth == p_width && RenderContext.ViewPortHeight == p_height)
		return;

	RenderContext.ViewPortX = p_x;
	RenderContext.ViewPortY = p_y;
	RenderContext.ViewPortWidth = p_width;
	RenderContext.ViewPortHeight = p_height;
}

void InitializeClippingFrustum()
{
	float z_near = 0.1f;
	float z_far = 100.0f;

	ClippingFrustum[0].Distance = 1.0f;
	ClippingFrustum[0].Normal.x = 1.0f;
	ClippingFrustum[0].Normal.y = 0.0f;
	ClippingFrustum[0].Normal.z = 0.0f;

	ClippingFrustum[1].Distance = 1.0f;
	ClippingFrustum[1].Normal.x = -1.0f;
	ClippingFrustum[1].Normal.y = 0.0f;
	ClippingFrustum[1].Normal.z = 0.0f;

	ClippingFrustum[2].Distance = 1.0f;
	ClippingFrustum[2].Normal.x = 0.0f;
	ClippingFrustum[2].Normal.y = 1.0f;
	ClippingFrustum[2].Normal.z = 0.0f;

	ClippingFrustum[3].Distance = 1.0f;
	ClippingFrustum[3].Normal.x = 0.0f;
	ClippingFrustum[3].Normal.y = -1.0f;
	ClippingFrustum[3].Normal.z = 0.0f;

	ClippingFrustum[4].Distance = z_near;
	ClippingFrustum[4].Normal.x = 0.0f;
	ClippingFrustum[4].Normal.y = 0.0f;
	ClippingFrustum[4].Normal.z = 1.0f;

	ClippingFrustum[5].Distance = z_far;
	ClippingFrustum[5].Normal.x = 0.0f;
	ClippingFrustum[5].Normal.y = 0.0f;
	ClippingFrustum[5].Normal.z = -1.0f;
}

void RasterizeTriangle(uint8_t p_primitiveMode, const AmberRenderer::Geometry::Vertex& p_vertex0, const AmberRenderer::Geometry::Vertex& p_vertex1, const AmberRenderer::Geometry::Vertex& p_vertex2)
{
	std::array<glm::vec4, 3> processedVertices{
		RenderContext.Shader->ProcessVertex(p_vertex0, 0),
		RenderContext.Shader->ProcessVertex(p_vertex1, 1),
		RenderContext.Shader->ProcessVertex(p_vertex2, 2)
	};

	if constexpr (CLIPPING)
	{
		AmberRenderer::Geometry::Polygon currentPoly;
		currentPoly.Vertices = { processedVertices[0], processedVertices[1], processedVertices[2] };
		currentPoly.TexCoords = { p_vertex0.texCoords, p_vertex1.texCoords, p_vertex2.texCoords };
		currentPoly.Normals = { p_vertex0.normal, p_vertex1.normal, p_vertex2.normal };
		currentPoly.VerticesCount = 3;

		auto& programVaryings = RenderContext.Shader->GetVaryings();

		int totalSize = 0;

		for (const auto& [name, varying] : programVaryings) 
		{
			totalSize += RenderContext.Shader->GetTypeCount(varying.Type);
		}

		currentPoly.VaryingsDataSize = totalSize;

		int offset = 0;
		for (auto& [name, varying] : programVaryings) 
		{
			uint8_t typeCount = RenderContext.Shader->GetTypeCount(varying.Type);

			for (uint8_t vertIdx = 0; vertIdx < 3; vertIdx++)
			{
				for (uint8_t i = 0; i < typeCount; i++)
				{
					currentPoly.Varyings[vertIdx][offset + i] = varying.Data[vertIdx][i];
				}
			}

			offset += typeCount;
		}

		for (const auto& plane : ClippingFrustum) 
		{
			ClipAgainstPlane(currentPoly, plane);
		}

		for (int i = 0; i < currentPoly.VerticesCount - 2; i++) 
		{
			std::array<glm::vec4, 3> clippedVertices{
				currentPoly.Vertices[0],
				currentPoly.Vertices[i + 1],
				currentPoly.Vertices[i + 2]
			};

			offset = 0;
			for (auto& [name, varying] : programVaryings) 
			{
				uint8_t typeCount = RenderContext.Shader->GetTypeCount(varying.Type);

				for (uint8_t j = 0; j < typeCount; j++) 
				{
					varying.Data[0][j] = currentPoly.Varyings[0][offset + j];
					varying.Data[1][j] = currentPoly.Varyings[i + 1][offset + j];
					varying.Data[2][j] = currentPoly.Varyings[i + 2][offset + j];
				}

				offset += typeCount;
			}

			TransformAndRasterizeVertices(p_primitiveMode, clippedVertices);
		}
	}
	else
	{
		TransformAndRasterizeVertices(p_primitiveMode, processedVertices);
	}
}

void TransformAndRasterizeVertices(const uint8_t p_primitiveMode, const std::array<glm::vec4, 3>& processedVertices)
{
	glm::vec3 vertexScreenPosition0 = ComputeScreenSpaceCoordinate(processedVertices[0]);
	glm::vec3 vertexScreenPosition1 = ComputeScreenSpaceCoordinate(processedVertices[1]);
	glm::vec3 vertexScreenPosition2 = ComputeScreenSpaceCoordinate(processedVertices[2]);

	glm::vec2 vertexNormalizedPosition0 = ComputeNormalizedDeviceCoordinate(vertexScreenPosition0);
	glm::vec2 vertexNormalizedPosition1 = ComputeNormalizedDeviceCoordinate(vertexScreenPosition1);
	glm::vec2 vertexNormalizedPosition2 = ComputeNormalizedDeviceCoordinate(vertexScreenPosition2);

	glm::vec2 vertexRasterPosition0 = ComputeRasterSpaceCoordinate(vertexNormalizedPosition0);
	glm::vec2 vertexRasterPosition1 = ComputeRasterSpaceCoordinate(vertexNormalizedPosition1);
	glm::vec2 vertexRasterPosition2 = ComputeRasterSpaceCoordinate(vertexNormalizedPosition2);

	std::array<glm::vec4, 3> transformedVertices{ processedVertices[0], processedVertices[1], processedVertices[2] };

	transformedVertices[0].x = vertexRasterPosition0.x;
	transformedVertices[0].y = vertexRasterPosition0.y;
	transformedVertices[0].z = vertexScreenPosition0.z;

	transformedVertices[1].x = vertexRasterPosition1.x;
	transformedVertices[1].y = vertexRasterPosition1.y;
	transformedVertices[1].z = vertexScreenPosition1.z;

	transformedVertices[2].x = vertexRasterPosition2.x;
	transformedVertices[2].y = vertexRasterPosition2.y;
	transformedVertices[2].z = vertexScreenPosition2.z;

	AmberRenderer::Geometry::Triangle triangle(vertexRasterPosition0, vertexRasterPosition1, vertexRasterPosition2);

	float area = triangle.ComputeArea();


	if ((RenderContext.CullFace == GLR_BACK && area > 0.0f) ||
		(RenderContext.CullFace == GLR_FRONT && area < 0.0f) ||
		RenderContext.CullFace == GLR_FRONT_AND_BACK)
		return;

	if (p_primitiveMode == GLR_TRIANGLES || p_primitiveMode == GLR_TRIANGLE_STRIP)
	{
		switch (RenderContext.PolygonMode)
		{
		case GLR_FILL:
			ComputeFragments(triangle, transformedVertices);
			break;
		case GLR_LINE:
			RasterizeTriangleWireframe(triangle, transformedVertices);
			break;
		case GLR_POINT:
			RasterizeTrianglePoints(triangle, transformedVertices);
			break;
		}
	}
	else if (p_primitiveMode == GLR_LINES)
	{
		RasterizeTriangleWireframe(triangle, transformedVertices);
	}
	else if (p_primitiveMode == GLR_POINTS)
	{
		RasterizeTrianglePoints(triangle, transformedVertices);
	}
}

void ComputeFragments(const AmberRenderer::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices)
{
	int xMin = std::max(p_triangle.BoundingBox2D.Min.x, 0);
	int yMin = std::max(p_triangle.BoundingBox2D.Min.y, 0);
	int xMax = std::min(p_triangle.BoundingBox2D.Max.x, static_cast<int32_t>(ActiveFrameBuffer ? ActiveFrameBuffer->Width : ActiveDepthBuffer->Width));
	int yMax = std::min(p_triangle.BoundingBox2D.Max.y, static_cast<int32_t>(ActiveFrameBuffer ? ActiveFrameBuffer->Height : ActiveDepthBuffer->Height));

	if (xMax <= xMin || yMax <= yMin)
		return;

	for (uint32_t x = xMin; x < xMax; x++)
	{
		for (uint32_t y = yMin; y < yMax; y++)
		{
			if (RenderContext.State & GLR_MULTISAMPLE && ActiveFrameBuffer)
			{
				uint8_t gridSize = static_cast<uint8_t>(std::ceil(std::sqrt(RenderContext.Samples)));

				uint8_t sampleCount = 0;

				for (uint8_t j = 0; j < gridSize; ++j)
				{
					for (uint8_t i = 0; i < gridSize; ++i)
					{
						if (sampleCount >= RenderContext.Samples)
							break;

						float samplePosX = x + (i + 0.5f) / gridSize;
						float samplePosY = y + (j + 0.5f) / gridSize;

						SetSampleFragment(p_triangle, x, y, samplePosX, samplePosY, sampleCount, transformedVertices);
						++sampleCount;
					}
				}
			}
			else
			{
				SetFragment(p_triangle, x, y, transformedVertices);
			}
		}
	}
}

void SetFragment(const AmberRenderer::Geometry::Triangle& p_triangle, uint32_t p_x, uint32_t p_y, const std::array<glm::vec4, 3>& p_transformedVertices)
{
	const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ p_x, p_y });

	if (barycentricCoords.x >= 0.0f && barycentricCoords.y >= 0.0f && barycentricCoords.x + barycentricCoords.y <= 1.0f)
	{
		float depth = p_transformedVertices[0].z * barycentricCoords.x +
			p_transformedVertices[1].z * barycentricCoords.y +
			p_transformedVertices[2].z * barycentricCoords.z;
		depth = depth * 0.5f + 0.5f;

		if (depth < 0.0f || depth > 1.0f)
			return;

		if (!(RenderContext.State & GLR_DEPTH_TEST) || depth <= ActiveDepthBuffer->GetPixel(p_x, p_y))
		{
			RenderContext.Shader->ProcessInterpolation(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

			AmberRenderer::Data::Color color = RenderContext.Shader->ProcessFragment();

			float alpha = color.a / 255.0f;

			if (ActiveFrameBuffer)
				ActiveFrameBuffer->SetPixel(p_x, p_y, AmberRenderer::Data::Color::Mix(AmberRenderer::Data::Color(ActiveFrameBuffer->GetPixel(p_x, p_y)), color, alpha));

			if (RenderContext.State & GLR_DEPTH_WRITE)
			{
				ActiveDepthBuffer->SetPixel(p_x, p_y, depth);
			}
		}
	}
}


void SetSampleFragment(const AmberRenderer::Geometry::Triangle& p_triangle, uint32_t p_x, uint32_t p_y, float p_sampleX, float p_sampleY, uint8_t p_sampleIndex, const std::array<glm::vec4, 3>& p_transformedVertices)
{
	const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ p_sampleX, p_sampleY });

	if (barycentricCoords.x >= 0.0f && barycentricCoords.y >= 0.0f && barycentricCoords.x + barycentricCoords.y <= 1.0f)
	{
		float depth = p_transformedVertices[0].z * barycentricCoords.x + p_transformedVertices[1].z * barycentricCoords.y + p_transformedVertices[2].z * barycentricCoords.z;

		depth = depth * 0.5f + 0.5f;

		if (depth < 0.0f || depth > 1.0f)
			return;

		if (!(RenderContext.State & GLR_DEPTH_TEST) || depth <= ActiveDepthBuffer->GetPixel(p_x, p_y))
		{
			RenderContext.Shader->ProcessInterpolation(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

			AmberRenderer::Data::Color color = RenderContext.Shader->ProcessFragment();

			if (RenderContext.State & GLR_MULTISAMPLE)
				MSAABuffer->SetPixelSample(p_x, p_y, p_sampleIndex, color, depth);
		}
	}
}

void RasterizeTriangleWireframe(const AmberRenderer::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices)
{
	RasterizeLine(p_triangle, transformedVertices, transformedVertices[0], transformedVertices[1]);
	RasterizeLine(p_triangle, transformedVertices, transformedVertices[1], transformedVertices[2]);
	RasterizeLine(p_triangle, transformedVertices, transformedVertices[2], transformedVertices[0]);
}

void RasterizeLine(const AmberRenderer::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices, const glm::vec4& p_start, const glm::vec4& p_end)
{
	int x0 = static_cast<int>(p_start.x);
	int y0 = static_cast<int>(p_start.y);
	int x1 = static_cast<int>(p_end.x);
	int y1 = static_cast<int>(p_end.y);

	// Bresenham's line algorithm.
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	int err = dx - dy;

	int width = static_cast<int>(ActiveFrameBuffer ? ActiveFrameBuffer->Width : ActiveDepthBuffer->Width);
	int height = static_cast<int>(ActiveFrameBuffer ? ActiveFrameBuffer->Height : ActiveDepthBuffer->Height);


	float totalDistance = sqrt(dx * dx + dy * dy);

	while (x0 != x1 || y0 != y1)
	{
		if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height)
		{
			const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ x0, y0 });

			float depth = transformedVertices[0].z * barycentricCoords.x + transformedVertices[1].z * barycentricCoords.y + transformedVertices[2].z * barycentricCoords.z;
			depth = depth * 0.5f + 0.5f;

			if (depth < 0.0f || depth > 1.0f)
				return;

			if (!(RenderContext.State & GLR_DEPTH_TEST) || depth <= ActiveDepthBuffer->GetPixel(x0, y0))
			{
				RenderContext.Shader->ProcessInterpolation(barycentricCoords, transformedVertices[0].w, transformedVertices[1].w, transformedVertices[2].w);

				AmberRenderer::Data::Color color = RenderContext.Shader->ProcessFragment();

				float alpha = color.a / 255.0f;

				if (ActiveFrameBuffer)
					ActiveFrameBuffer->SetPixel(x0, y0, AmberRenderer::Data::Color::Mix(AmberRenderer::Data::Color(ActiveFrameBuffer->GetPixel(x0, y0)), color, alpha));

				if (RenderContext.State & GLR_DEPTH_WRITE)
				{
					ActiveDepthBuffer->SetPixel(x0, y0, depth);
				}
			}
		}

		int e2 = (err << 1);

		if (e2 > -dy)
		{
			err -= dy;
			x0 += sx;
		}

		if (e2 < dx)
		{
			err += dx;
			y0 += sy;
		}
	}
}

void RasterizeLine(const glm::vec4& p_start, const glm::vec4& p_end, const AmberRenderer::Data::Color& p_color)
{
	int x0 = static_cast<int>(p_start.x);
	int y0 = static_cast<int>(p_start.y);
	int x1 = static_cast<int>(p_end.x);
	int y1 = static_cast<int>(p_end.y);

	// Bresenham's line algorithm.
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	int err = dx - dy;

	int width = static_cast<int>(ActiveFrameBuffer ? ActiveFrameBuffer->Width : ActiveDepthBuffer->Width);
	int height = static_cast<int>(ActiveFrameBuffer ? ActiveFrameBuffer->Height : ActiveDepthBuffer->Height);


	float totalDistance = sqrt(dx * dx + dy * dy);

	while (x0 != x1 || y0 != y1)
	{
		float currentDistance = sqrt((x0 - p_start.x) * (x0 - p_start.x) + (y0 - p_end.y) * (y0 - p_end.y));

		float depth = p_start.z * ((totalDistance - currentDistance) / totalDistance) + p_end.z * (currentDistance / totalDistance);
		depth = depth * 0.5f + 0.5f;

		if (depth < 0.0f || depth > 1.0f)
			return;

		if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height)
		{
			if (!(RenderContext.State & GLR_DEPTH_TEST) || depth <= ActiveDepthBuffer->GetPixel(x0, y0))
			{
				if (ActiveFrameBuffer)
					ActiveFrameBuffer->SetPixel(x0, y0, p_color);

				if (RenderContext.State & GLR_DEPTH_WRITE)
				{
					ActiveDepthBuffer->SetPixel(x0, y0, depth);
				}
			}
		}

		int e2 = (err << 1);

		if (e2 > -dy)
		{
			err -= dy;
			x0 += sx;
		}

		if (e2 < dx)
		{
			err += dx;
			y0 += sy;
		}
	}
}

void RasterizeTrianglePoints(const AmberRenderer::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices)
{
	DrawPoint(p_triangle, transformedVertices, transformedVertices[0]);
	DrawPoint(p_triangle, transformedVertices, transformedVertices[1]);
	DrawPoint(p_triangle, transformedVertices, transformedVertices[2]);
}

void DrawPoint(const AmberRenderer::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices, const glm::vec4& p_point)
{
	int width = static_cast<int>(ActiveFrameBuffer ? ActiveFrameBuffer->Width : ActiveDepthBuffer->Width);
	int height = static_cast<int>(ActiveFrameBuffer ? ActiveFrameBuffer->Height : ActiveDepthBuffer->Height);


	if (p_point.x >= 0 && p_point.x < width && p_point.y >= 0 && p_point.y < height)
	{
		const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ p_point.x, p_point.y });

		float depth = transformedVertices[0].z * barycentricCoords.x +
			transformedVertices[1].z * barycentricCoords.y +
			transformedVertices[2].z * barycentricCoords.z;
		depth = depth * 0.5f + 0.5f;

		if (depth < 0.0f || depth > 1.0f)
			return;

		if (!(RenderContext.State & GLR_DEPTH_TEST) || depth <= ActiveDepthBuffer->GetPixel(p_point.x, p_point.y))
		{
			RenderContext.Shader->ProcessInterpolation(barycentricCoords, transformedVertices[0].w, transformedVertices[1].w, transformedVertices[2].w);

			AmberRenderer::Data::Color color = RenderContext.Shader->ProcessFragment();

			float alpha = color.a / 255.0f;

			if (ActiveFrameBuffer)
				ActiveFrameBuffer->SetPixel(p_point.x, p_point.y, AmberRenderer::Data::Color::Mix(AmberRenderer::Data::Color(ActiveFrameBuffer->GetPixel(p_point.x, p_point.y)), color, alpha));

			if (RenderContext.State & GLR_DEPTH_WRITE)
			{
				ActiveDepthBuffer->SetPixel(p_point.x, p_point.y, depth);
			}
		}
	}
}

void DrawPoint(const glm::vec2& p_point, const AmberRenderer::Data::Color& p_color)
{
	if (!ActiveFrameBuffer)
		return;

	int width = static_cast<int>(ActiveFrameBuffer->Width);
	int height = static_cast<int>(ActiveFrameBuffer->Height);

	if (p_point.x >= 0 && p_point.x < width && p_point.y >= 0 && p_point.y < height)
	{
		ActiveFrameBuffer->SetPixel(p_point.x, p_point.y, p_color);
	}
}

void RasterizeLine(const AmberRenderer::Geometry::Vertex& p_vertex0, const AmberRenderer::Geometry::Vertex& p_vertex1, const AmberRenderer::Data::Color& p_color)
{
	glm::vec4 vertexWorldPosition0 = RenderContext.Shader->ProcessVertex(p_vertex0, 0);
	glm::vec4 vertexWorldPosition1 = RenderContext.Shader->ProcessVertex(p_vertex1, 1);

	glm::vec3 vertexScreenPosition0 = ComputeScreenSpaceCoordinate(vertexWorldPosition0);
	glm::vec3 vertexScreenPosition1 = ComputeScreenSpaceCoordinate(vertexWorldPosition1);

	glm::vec2 vertexNormalizedPosition0 = ComputeNormalizedDeviceCoordinate(vertexScreenPosition0);
	glm::vec2 vertexNormalizedPosition1 = ComputeNormalizedDeviceCoordinate(vertexScreenPosition1);

	glm::vec2 vertexRasterPosition0 = ComputeRasterSpaceCoordinate(vertexNormalizedPosition0);
	glm::vec2 vertexRasterPosition1 = ComputeRasterSpaceCoordinate(vertexNormalizedPosition1);

	int x0 = static_cast<int>(vertexRasterPosition0.x);
	int y0 = static_cast<int>(vertexRasterPosition0.y);
	int x1 = static_cast<int>(vertexRasterPosition1.x);
	int y1 = static_cast<int>(vertexRasterPosition1.y);

	// Bresenham's line algorithm.
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	int err = dx - dy;

	int width = static_cast<int>(ActiveFrameBuffer ? ActiveFrameBuffer->Width : ActiveDepthBuffer->Width);
	int height = static_cast<int>(ActiveFrameBuffer ? ActiveFrameBuffer->Height : ActiveDepthBuffer->Height);


	float totalDistance = sqrt(dx * dx + dy * dy);

	while (x0 != x1 || y0 != y1)
	{
		float currentDistance = sqrt((x0 - vertexRasterPosition0.x) * (x0 - vertexRasterPosition0.x) + (y0 - vertexRasterPosition0.y) * (y0 - vertexRasterPosition0.y));

		float depth = vertexScreenPosition0.z * ((totalDistance - currentDistance) / totalDistance) + vertexScreenPosition1.z * (currentDistance / totalDistance);
		depth = depth * 0.5f + 0.5f;

		if (depth < 0.0f || depth > 1.0f)
			return;

		if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height)
		{
			if (!(RenderContext.State & GLR_DEPTH_TEST) || depth <= ActiveDepthBuffer->GetPixel(x0, y0))
			{
				if (ActiveFrameBuffer)
					ActiveFrameBuffer->SetPixel(x0, y0, p_color);

				if (RenderContext.State & GLR_DEPTH_WRITE)
				{
					ActiveDepthBuffer->SetPixel(x0, y0, depth);
				}
			}
		}

		int e2 = (err << 1);

		if (e2 > -dy)
		{
			err -= dy;
			x0 += sx;
		}

		if (e2 < dx)
		{
			err += dx;
			y0 += sy;
		}
	}
}

glm::vec3 ComputeScreenSpaceCoordinate(const glm::vec4& p_vertexWorldPosition)
{
	return p_vertexWorldPosition / p_vertexWorldPosition.w;
}

glm::vec2 ComputeNormalizedDeviceCoordinate(const glm::vec3& p_vertexScreenSpacePosition)
{
	glm::vec2 normalizedCoordinate;

	normalizedCoordinate.x = (p_vertexScreenSpacePosition.x + 1.0f) * 0.5f;
	normalizedCoordinate.y = (1.0f - p_vertexScreenSpacePosition.y) * 0.5f;

	return normalizedCoordinate;
}

glm::vec2 ComputeRasterSpaceCoordinate(glm::vec2 p_vertexNormalizedPosition) 
{
	p_vertexNormalizedPosition.x = RenderContext.ViewPortX + p_vertexNormalizedPosition.x * RenderContext.ViewPortWidth - 0.5f;
	p_vertexNormalizedPosition.y = RenderContext.ViewPortY + p_vertexNormalizedPosition.y * RenderContext.ViewPortHeight - 0.5f;

	return p_vertexNormalizedPosition;
}

void ClipAgainstPlane(AmberRenderer::Geometry::Polygon& p_polygon, const AmberRenderer::Geometry::Plane& p_plane)
{
	if (p_polygon.VerticesCount == 0)
		return;

	glm::vec4 plane4D = glm::vec4(
		p_plane.Normal.x,
		p_plane.Normal.y,
		p_plane.Normal.z,
		1.0f
	);

	glm::vec4 insideVertices[AmberRenderer::Geometry::MAX_POLY_COUNT];
	glm::vec2 insideTexCoords[AmberRenderer::Geometry::MAX_POLY_COUNT];
	glm::vec3 insideNormals[AmberRenderer::Geometry::MAX_POLY_COUNT];
	float insideVaryings[AmberRenderer::Geometry::MAX_POLY_COUNT][16];

	uint8_t insideVerticesCount = 0;

	glm::vec4 previousVertex = p_polygon.Vertices[p_polygon.VerticesCount - 1];
	glm::vec2 previousTexCoords = p_polygon.TexCoords[p_polygon.VerticesCount - 1];
	glm::vec3 previousNormal = p_polygon.Normals[p_polygon.VerticesCount - 1];
	float* previousVarying = p_polygon.Varyings[p_polygon.VerticesCount - 1];

	float previousDotValue = glm::dot(previousVertex, plane4D);

	for (int i = 0; i < p_polygon.VerticesCount; i++)
	{
		float currentDotValue = glm::dot(p_polygon.Vertices[i], plane4D);

		if (previousDotValue * currentDotValue < 0.0f)
		{
			float t = previousDotValue / (previousDotValue - currentDotValue);

			insideVertices[insideVerticesCount] = {
				glm::lerp(previousVertex.x, p_polygon.Vertices[i].x, t),
				glm::lerp(previousVertex.y, p_polygon.Vertices[i].y, t),
				glm::lerp(previousVertex.z, p_polygon.Vertices[i].z, t),
				glm::lerp(previousVertex.w, p_polygon.Vertices[i].w, t)
			};

			insideTexCoords[insideVerticesCount] = {
				glm::lerp(previousTexCoords.x, p_polygon.TexCoords[i].x, t),
				glm::lerp(previousTexCoords.y, p_polygon.TexCoords[i].y, t)
			};

			insideNormals[insideVerticesCount] = {
				glm::lerp(previousNormal.x, p_polygon.Normals[i].x, t),
				glm::lerp(previousNormal.y, p_polygon.Normals[i].y, t),
				glm::lerp(previousNormal.z, p_polygon.Normals[i].z, t)
			};

			for (int j = 0; j < p_polygon.VaryingsDataSize; j++)
			{
				insideVaryings[insideVerticesCount][j] = glm::lerp(previousVarying[j], p_polygon.Varyings[i][j], t);
			}

			insideVerticesCount++;
		}

		if (currentDotValue >= 0.0f)
		{
			insideVertices[insideVerticesCount] = p_polygon.Vertices[i];
			insideTexCoords[insideVerticesCount] = p_polygon.TexCoords[i];
			insideNormals[insideVerticesCount] = p_polygon.Normals[i];

			for (int j = 0; j < p_polygon.VaryingsDataSize; j++)
			{
				insideVaryings[insideVerticesCount][j] = p_polygon.Varyings[i][j];
			}

			insideVerticesCount++;
		}

		previousVertex = p_polygon.Vertices[i];
		previousTexCoords = p_polygon.TexCoords[i];
		previousNormal = p_polygon.Normals[i];
		previousVarying = p_polygon.Varyings[i];
		previousDotValue = currentDotValue;
	}

	p_polygon.VerticesCount = insideVerticesCount;

	for (int i = 0; i < insideVerticesCount; i++)
	{
		p_polygon.Vertices[i] = insideVertices[i];
		p_polygon.TexCoords[i] = insideTexCoords[i];
		p_polygon.Normals[i] = insideNormals[i];

		for (int j = 0; j < p_polygon.VaryingsDataSize; j++)
		{
			p_polygon.Varyings[i][j] = insideVaryings[i][j];
		}
	}
}

void ApplyMSAA()
{
	if (!ActiveFrameBuffer)
		return;

	const uint32_t width = ActiveFrameBuffer->Width;
	const uint32_t height = ActiveFrameBuffer->Height;

	float depth = 0.0f;

	for (uint32_t x = 0; x < width; x++)
	{
		for (uint32_t y = 0; y < height; y++)
		{
			glm::ivec4 color = { 0, 0, 0, 0 };

			depth = 0.0f;

			for (uint8_t sampleIndex = 0; sampleIndex < RenderContext.Samples; ++sampleIndex)
			{
				const auto& sample = MSAABuffer->GetSample(x, y, sampleIndex);

				color.x += static_cast<uint8_t>(sample.Color >> 24);
				color.y += static_cast<uint8_t>(sample.Color >> 16);
				color.z += static_cast<uint8_t>(sample.Color >> 8);
				color.w += static_cast<uint8_t>(sample.Color);

				depth += sample.Depth;
			}

			color.x /= RenderContext.Samples;
			color.y /= RenderContext.Samples;
			color.z /= RenderContext.Samples;
			color.w /= RenderContext.Samples;

			depth /= static_cast<float>(RenderContext.Samples);

			AmberRenderer::Data::Color sampledColorTotal(static_cast<uint8_t>(color.x), static_cast<uint8_t>(color.y), static_cast<uint8_t>(color.z), static_cast<uint8_t>(color.w));
			const float alpha = static_cast<float>(sampledColorTotal.a) / 255.0f;

			ActiveFrameBuffer->SetPixel(x, y, AmberRenderer::Data::Color::Mix(AmberRenderer::Data::Color(ActiveFrameBuffer->GetPixel(x, y)), sampledColorTotal, alpha));

			if (RenderContext.State & GLR_DEPTH_WRITE)
			{
				ActiveDepthBuffer->SetPixel(x, y, depth);
			}
		}
	}
}