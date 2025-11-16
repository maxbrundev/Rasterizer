#include "AmberGL/SoftwareRenderer/TextureSampler.h"

#include <algorithm>

#include "AmberGL/Maths/Uttils/MathUtils.h"
#include "AmberGL/SoftwareRenderer/Defines.h"

glm::vec4 AmberGL::SoftwareRenderer::TextureSampler::Sample(const RenderObject::TextureObject* p_textureObject, const glm::vec2& p_texCoords, const glm::vec2& p_dfdx, const glm::vec2& p_dfdy)
{
	if (p_textureObject == nullptr)
		return glm::vec4(1.0f);

	uint32_t width = p_textureObject->Width;
	uint32_t height = p_textureObject->Height;

	bool hasMipmaps = p_textureObject->Mipmaps != nullptr;
	uint8_t currentLOD = 0;

	if (hasMipmaps)
	{
		currentLOD = ComputeMipmapLevel(p_textureObject, p_dfdx, p_dfdy);
	}

	/*switch (currentLOD)
	{
		case 0:  return glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
		case 1:  return glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
		case 2:  return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue
		case 3:  return glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
		case 4:  return glm::vec4(1.0f, 0.0f, 1.0f, 1.0f); // Magenta
		case 5:  return glm::vec4(0.0f, 1.0f, 1.0f, 1.0f); // Cyan
		case 6:  return glm::vec4(1.0f, 0.5f, 0.0f, 1.0f); // Orange
		case 7:  return glm::vec4(0.5f, 0.0f, 1.0f, 1.0f); // Purple
		case 8:  return glm::vec4(0.5f, 0.5f, 0.5f, 1.0f); // Gray
		case 9:  return glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // White
		case 10: return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Black
		default: return glm::vec4(0.2f, 0.2f, 0.2f, 1.0f); // Fallback color
	}*/

	if (hasMipmaps && currentLOD > 0)
	{
		width = std::max<uint32_t>(1, width >> currentLOD);
		height = std::max<uint32_t>(1, height >> currentLOD);
	}

	float uvX = std::abs(p_texCoords.x);
	float uvY = std::abs(p_texCoords.y);

	uvX = ApplyWrapMode(uvX, p_textureObject->WrapS);
	uvY = ApplyWrapMode(uvY, p_textureObject->WrapT);

	uvX = uvX * (static_cast<float>(width) - 0.5f);
	uvY = uvY * (static_cast<float>(height) - 0.5f);

	uint16_t filter = currentLOD == 0 ? p_textureObject->MagFilter : p_textureObject->MinFilter;


	if (p_textureObject->InternalFormat == AGL_R32F && p_textureObject->Data32)
	{
		const float* data = p_textureObject->Data32; // No mipmap support for float yet

		if (filter == AGL_LINEAR)
		{
			return SampleBilinear(data, width, height, uvX, uvY);
		}

		return SampleNearest(data, width, height, uvX, uvY);
	}

	const uint8_t* data = hasMipmaps && currentLOD > 0 ? p_textureObject->Mipmaps[currentLOD] : p_textureObject->Data8;

	if (filter == AGL_LINEAR)
	{
		return SampleBilinear(data, width, height, uvX, uvY);
	}

	return SampleNearest(data, width, height, uvX, uvY);
}

void AmberGL::SoftwareRenderer::TextureSampler::GenerateMipmaps(RenderObject::TextureObject* p_textureObject)
{
	int currentLevelWidth = p_textureObject->Width;
	int currentLevelHeight = p_textureObject->Height;

	const int totalMipmapLevels = 1 + static_cast<int>(std::floor(std::log2(std::max(currentLevelWidth, currentLevelHeight))));

	p_textureObject->Mipmaps = new uint8_t * [totalMipmapLevels];

	p_textureObject->Mipmaps[0] = new uint8_t[currentLevelWidth * currentLevelHeight * 4];
	std::memcpy(p_textureObject->Mipmaps[0], p_textureObject->Data8, currentLevelWidth * currentLevelHeight * 4);

	for (int mipmapLevel = 1; mipmapLevel < totalMipmapLevels; mipmapLevel++)
	{
		int previousLevelWidth = currentLevelWidth;
		int previousLevelHeight = currentLevelHeight;

		currentLevelWidth = std::max(1, currentLevelWidth / 2);
		currentLevelHeight = std::max(1, currentLevelHeight / 2);

		p_textureObject->Mipmaps[mipmapLevel] = new uint8_t[currentLevelWidth * currentLevelHeight * 4];

		uint8_t* previousLevelData = p_textureObject->Mipmaps[mipmapLevel - 1];
		uint8_t* currentLevelData = p_textureObject->Mipmaps[mipmapLevel];

		for (int pixelY = 0; pixelY < currentLevelHeight; pixelY++)
		{
			for (int pixelX = 0; pixelX < currentLevelWidth; pixelX++)
			{
				int sourcePosX = pixelX * 2;
				int sourcePosY = pixelY * 2;

				int sourcePosXEnd = std::min(sourcePosX + 2, previousLevelWidth);
				int sourcePosYEnd = std::min(sourcePosY + 2, previousLevelHeight);

				int sourcePixelCount = (sourcePosXEnd - sourcePosX) * (sourcePosYEnd - sourcePosY);

				float colorChannelSums[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

				for (int sourceY = sourcePosY; sourceY < sourcePosYEnd; sourceY++)
				{
					for (int sourceX = sourcePosX; sourceX < sourcePosXEnd; sourceX++)
					{
						int sourcePixelIndex = (sourceY * previousLevelWidth + sourceX) * 4;
						for (int channel = 0; channel < 4; channel++)
						{
							colorChannelSums[channel] += previousLevelData[sourcePixelIndex + channel];
						}
					}
				}

				int currentPixelIndex = (pixelY * currentLevelWidth + pixelX) * 4;

				for (int channel = 0; channel < 4; channel++)
				{
					currentLevelData[currentPixelIndex + channel] = static_cast<uint8_t>(colorChannelSums[channel] / sourcePixelCount);
				}
			}
		}
	}
}

uint8_t AmberGL::SoftwareRenderer::TextureSampler::ComputeMipmapLevel(const RenderObject::TextureObject* p_textureObject, const glm::vec2& p_dfdx, const glm::vec2& p_dfdy)
{
	if (!p_textureObject || !p_textureObject->Mipmaps)
		return 0;

	glm::vec2 textureSize(p_textureObject->Width, p_textureObject->Height);
	glm::vec2 tcDx = p_dfdx * textureSize;
	glm::vec2 tcDy = p_dfdy * textureSize;

	// Compute the maximum rate of change (matching standard GPU behavior)
	float deltaMaxSqr = std::max(
		tcDx.x * tcDx.x + tcDx.y * tcDx.y,
		tcDy.x * tcDy.x + tcDy.y * tcDy.y
	);

	// Handle edge cases
	if (deltaMaxSqr <= 0.0f) {
		return 0; // Use highest detail level for zero or negative derivatives
	}

	// Compute mip level using log2 of the maximum derivative
	float level = 0.5f * std::log2(deltaMaxSqr);

	// Clamp to valid mipmap range
	int maxMipLevel = static_cast<int>(std::floor(std::log2(std::max(p_textureObject->Width, p_textureObject->Height))));
	level = std::clamp(level, 0.0f, static_cast<float>(maxMipLevel));

	return static_cast<uint8_t>(std::round(level));
}

float AmberGL::SoftwareRenderer::TextureSampler::ApplyWrapMode(float p_texCoord, uint16_t p_wrapMode)
{
	if (p_wrapMode == AGL_CLAMP)
	{
		return glm::clamp(p_texCoord, 0.0f, 1.0f);
	}

	if (p_wrapMode == AGL_REPEAT)
	{
		return glm::mod(p_texCoord, 1.0f);
	}

	return glm::clamp(p_texCoord, 0.0f, 1.0f);
}

glm::vec4 AmberGL::SoftwareRenderer::TextureSampler::SampleNearest(const uint8_t* p_data, uint32_t p_width, uint32_t p_height, float p_x, float p_y)
{
	int x = static_cast<int>(std::round(p_x));
	int y = static_cast<int>(std::round(p_y));

	x = glm::clamp(x, 0, static_cast<int>(p_width) - 1);
	y = glm::clamp(y, 0, static_cast<int>(p_height) - 1);

	y = (p_height - 1) - y;

	const uint32_t index = (y * p_width + x) * 4;

	return glm::vec4(
		p_data[index] / 255.0f,
		p_data[index + 1] / 255.0f,
		p_data[index + 2] / 255.0f,
		p_data[index + 3] / 255.0f
	);
}

glm::vec4 AmberGL::SoftwareRenderer::TextureSampler::SampleBilinear(const uint8_t* p_data, uint32_t p_width, glm::uint32_t p_height, float p_x, float p_y)
{
	p_y = (p_height - 1.0f) - p_y;

	int x0 = static_cast<int>(std::floor(p_x));
	int y0 = static_cast<int>(std::floor(p_y));
	int x1 = std::min(x0 + 1, static_cast<int>(p_width) - 1);
	int y1 = std::min(y0 + 1, static_cast<int>(p_height) - 1);

	float fracX = p_x - x0;
	float fracY = p_y - y0;

	uint32_t idx00 = (y0 * p_width + x0) * 4;
	uint32_t idx01 = (y1 * p_width + x0) * 4;
	uint32_t idx10 = (y0 * p_width + x1) * 4;
	uint32_t idx11 = (y1 * p_width + x1) * 4;

	glm::vec4 color;

	for (int i = 0; i < 4; i++)
	{
		float c00 = p_data[idx00 + i] / 255.0f;
		float c01 = p_data[idx01 + i] / 255.0f;
		float c10 = p_data[idx10 + i] / 255.0f;
		float c11 = p_data[idx11 + i] / 255.0f;

		color[i] = MathUtils::InterpolateBilinear(c00, c10, c01, c11, fracX, fracY);
	}

	return color;
}

glm::vec4 AmberGL::SoftwareRenderer::TextureSampler::SampleNearest(const float* p_data, uint32_t p_width,
	uint32_t p_height, float p_x, float p_y)
{
	int x = static_cast<int>(std::round(p_x));
	int y = static_cast<int>(std::round(p_y));

	x = glm::clamp(x, 0, static_cast<int>(p_width) - 1);
	y = glm::clamp(y, 0, static_cast<int>(p_height) - 1);

	y = (p_height - 1) - y; // Flip Y

	const uint32_t index = y * p_width + x;
	float value = p_data[index];

	// R32F returns single channel as grayscale
	return glm::vec4(value, value, value, 1.0f);
}

glm::vec4 AmberGL::SoftwareRenderer::TextureSampler::SampleBilinear(const float* p_data, uint32_t p_width,
	glm::uint32_t p_height, float p_x, float p_y)
{
	p_y = (p_height - 1.0f) - p_y; // Flip Y

	int x0 = static_cast<int>(std::floor(p_x));
	int y0 = static_cast<int>(std::floor(p_y));
	int x1 = std::min(x0 + 1, static_cast<int>(p_width) - 1);
	int y1 = std::min(y0 + 1, static_cast<int>(p_height) - 1);

	float fracX = p_x - x0;
	float fracY = p_y - y0;

	uint32_t idx00 = y0 * p_width + x0;
	uint32_t idx01 = y1 * p_width + x0;
	uint32_t idx10 = y0 * p_width + x1;
	uint32_t idx11 = y1 * p_width + x1;

	float c00 = p_data[idx00];
	float c01 = p_data[idx01];
	float c10 = p_data[idx10];
	float c11 = p_data[idx11];

	// Bilinear interpolation
	float value = (1.0f - fracX) * (1.0f - fracY) * c00 +
		fracX * (1.0f - fracY) * c10 +
		(1.0f - fracX) * fracY * c01 +
		fracX * fracY * c11;

	// R32F returns single channel as grayscale
	return glm::vec4(value, value, value, 1.0f);
}
