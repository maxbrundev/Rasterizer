#include "AmberGL/SoftwareRenderer/TextureSampler.h"

#include <algorithm>

#include "AmberGL/Maths/Uttils/MathUtils.h"
#include "AmberGL/SoftwareRenderer/Defines.h"

glm::vec4 AmberGL::SoftwareRenderer::TextureSampler::Sample(const RenderObject::TextureObject* p_textureObject, const glm::vec2& p_texCoords, const std::array<glm::vec2, 2>& p_texCoordsDerivatives)
{
	if (p_textureObject == nullptr)
		return glm::vec4(1.0f);

	uint32_t width = p_textureObject->Width;
	uint32_t height = p_textureObject->Height;

	bool hasMipmaps = p_textureObject->Mipmaps != nullptr;
	uint8_t currentLOD = 0;

	if (hasMipmaps)
	{
		currentLOD = ComputeMipmapLevel(p_textureObject, p_texCoordsDerivatives[0], p_texCoordsDerivatives[1]);
	}

	if (hasMipmaps && currentLOD > 0)
	{
		width = std::max(1u, width >> currentLOD);
		height = std::max(1u, height >> currentLOD);
	}

	float uvX = std::abs(p_texCoords.x);
	float uvY = std::abs(p_texCoords.y);

	uvX = ApplyWrapMode(uvX, p_textureObject->WrapS);
	uvY = ApplyWrapMode(uvY, p_textureObject->WrapT);

	uvX = uvX * (static_cast<float>(width) - 0.5f);
	uvY = uvY * (static_cast<float>(height) - 0.5f);

	uint8_t filter = currentLOD == 0 ? p_textureObject->MagFilter : p_textureObject->MinFilter;

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

	float det = std::abs(tcDx.x * tcDy.y - tcDx.y * tcDy.x);

	float texelArea = 1.0f / det;

	bool magnification = texelArea >= 1.0f;

	if (magnification)
		return 0;
	
	uint8_t mipmapCurrentLevel = std::ceil(-std::log2(std::min(1.0f, texelArea)) / 2.0f);

	uint8_t mipmapMaxLevel = 1 + static_cast<int>(std::floor(std::log2(std::max(p_textureObject->Width, p_textureObject->Height))));

	return std::min<uint8_t>(mipmapCurrentLevel, mipmapMaxLevel - 1);
}

float AmberGL::SoftwareRenderer::TextureSampler::ApplyWrapMode(float p_coord, uint8_t p_wrapMode)
{
	if (p_wrapMode == AGL_CLAMP)
	{
		return glm::clamp(p_coord, 0.0f, 1.0f);
	}

	if (p_wrapMode == AGL_REPEAT)
	{
		return glm::mod(p_coord, 1.0f);
	}

	return glm::clamp(p_coord, 0.0f, 1.0f);
}

glm::vec4 AmberGL::SoftwareRenderer::TextureSampler::SampleNearest(const uint8_t* p_data, uint32_t p_width, uint32_t p_height, float p_x, float p_y)
{
	int x = static_cast<int>(std::round(p_x));
	int y = static_cast<int>(std::round(p_y));

	x = glm::clamp(x, 0, static_cast<int>(p_width) - 1);
	y = glm::clamp(y, 0, static_cast<int>(p_height) - 1);

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
