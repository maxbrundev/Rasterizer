#pragma once

#include <array>

#include <AmberGL/SoftwareRenderer/Defines.h>

#include "Settings/EBufferType.h"
#include "Settings/EClearFlag.h"
#include "Settings/EComparisonOperand.h"
#include "Settings/ECullFace.h"
#include "Settings/EFilterMode.h"
#include "Settings/EFramebufferAttachment.h"
#include "Settings/EPixelFormat.h"
#include "Settings/EPolygonMode.h"
#include "Settings/EPrimitiveMode.h"
#include "Settings/ERenderingCapability.h"
#include "Settings/EUniformType.h"
#include "Settings/EViewport.h"

template<typename Enum, typename Value>
struct EnumValueTypeTraits
{
};

template <>
struct EnumValueTypeTraits<AmberEditor::Rendering::Settings::EClearFlag, uint16_t>
{
	static constexpr std::array<std::pair<AmberEditor::Rendering::Settings::EClearFlag, uint16_t>, 2> EnumMap =
	{
		std::pair{AmberEditor::Rendering::Settings::EClearFlag::COLOR, AGL_COLOR_BUFFER_BIT},
		std::pair{AmberEditor::Rendering::Settings::EClearFlag::DEPTH, AGL_DEPTH_BUFFER_BIT}
	};
};

template <>
struct EnumValueTypeTraits<AmberEditor::Rendering::Settings::EPolygonMode, uint16_t>
{
	static constexpr std::array<std::pair<AmberEditor::Rendering::Settings::EPolygonMode, uint16_t>, 3> EnumMap =
	{
		std::pair{AmberEditor::Rendering::Settings::EPolygonMode::FILL,  AGL_FILL},
		std::pair{AmberEditor::Rendering::Settings::EPolygonMode::LINE,  AGL_LINE},
		std::pair{AmberEditor::Rendering::Settings::EPolygonMode::POINT, AGL_POINT}
	};
};

template <>
struct EnumValueTypeTraits<AmberEditor::Rendering::Settings::EPrimitiveMode, uint16_t>
{
	static constexpr std::array<std::pair<AmberEditor::Rendering::Settings::EPrimitiveMode, uint16_t>, 4> EnumMap =
	{
		std::pair{AmberEditor::Rendering::Settings::EPrimitiveMode::LINES,          AGL_LINES},
		std::pair{AmberEditor::Rendering::Settings::EPrimitiveMode::POINTS,         AGL_POINTS},
		std::pair{AmberEditor::Rendering::Settings::EPrimitiveMode::TRIANGLES,      AGL_TRIANGLES},
		std::pair{AmberEditor::Rendering::Settings::EPrimitiveMode::TRIANGLE_STRIP, AGL_TRIANGLE_STRIP}
	};
};

template <>
struct EnumValueTypeTraits<AmberEditor::Rendering::Settings::ECullFace, uint16_t>
{
	static constexpr std::array<std::pair<AmberEditor::Rendering::Settings::ECullFace, uint16_t>, 3> EnumMap =
	{
		std::pair{AmberEditor::Rendering::Settings::ECullFace::BACK,           AGL_BACK},
		std::pair{AmberEditor::Rendering::Settings::ECullFace::FRONT,          AGL_FRONT},
		std::pair{AmberEditor::Rendering::Settings::ECullFace::FRONT_AND_BACK, AGL_FRONT_AND_BACK
		}
	};
};

template <>
struct EnumValueTypeTraits<AmberEditor::Rendering::Settings::ERenderingCapability, uint16_t>
{
	static constexpr std::array<std::pair<AmberEditor::Rendering::Settings::ERenderingCapability, uint16_t>, 4> EnumMap =
	{
		std::pair{AmberEditor::Rendering::Settings::ERenderingCapability::DEPTH_WRITE, AGL_BACK},
		std::pair{AmberEditor::Rendering::Settings::ERenderingCapability::DEPTH_TEST,  AGL_FRONT},
		std::pair{AmberEditor::Rendering::Settings::ERenderingCapability::CULL_FACE,   AGL_FRONT_AND_BACK},
		std::pair{AmberEditor::Rendering::Settings::ERenderingCapability::MULTISAMPLE, AGL_MULTISAMPLE}
	};
};

template <>
struct EnumValueTypeTraits<AmberEditor::Rendering::Settings::EBufferType, uint16_t>
{
	static constexpr std::array<std::pair<AmberEditor::Rendering::Settings::EBufferType, uint16_t>, 2> EnumMap =
	{
		std::pair{AmberEditor::Rendering::Settings::EBufferType::VERTEX, AGL_ARRAY_BUFFER},
		std::pair{AmberEditor::Rendering::Settings::EBufferType::INDEX,  AGL_ELEMENT_ARRAY_BUFFER}
	};
};

template <>
struct EnumValueTypeTraits<AmberEditor::Rendering::Settings::EComparisonOperand, uint16_t>
{
	static constexpr std::array<std::pair<AmberEditor::Rendering::Settings::EComparisonOperand, uint16_t>, 8> EnumMap =
	{
		std::pair{AmberEditor::Rendering::Settings::EComparisonOperand::NEVER,         AGL_NEVER},
		std::pair{AmberEditor::Rendering::Settings::EComparisonOperand::LESS,          AGL_LESS},
		std::pair{AmberEditor::Rendering::Settings::EComparisonOperand::EQUAL,         AGL_EQUAL},
		std::pair{AmberEditor::Rendering::Settings::EComparisonOperand::LESS_EQUAL,    AGL_LEQUAL},
		std::pair{AmberEditor::Rendering::Settings::EComparisonOperand::GREATER,       AGL_GREATER},
		std::pair{AmberEditor::Rendering::Settings::EComparisonOperand::NOT_EQUAL,     AGL_NOTEQUAL},
		std::pair{AmberEditor::Rendering::Settings::EComparisonOperand::GREATER_EQUAL, AGL_GEQUAL},
		std::pair{AmberEditor::Rendering::Settings::EComparisonOperand::ALWAYS,        AGL_ALWAYS}
	};
};

template <>
struct EnumValueTypeTraits<AmberEditor::Rendering::Settings::EFilterMode, uint16_t>
{
	static constexpr std::array<std::pair<AmberEditor::Rendering::Settings::EFilterMode, uint16_t>, 2> EnumMap =
	{
		std::pair{AmberEditor::Rendering::Settings::EFilterMode::LINEAR,  AGL_LINEAR},
		std::pair{AmberEditor::Rendering::Settings::EFilterMode::NEAREST, AGL_NEAREST}
	};
};

template <>
struct EnumValueTypeTraits<AmberEditor::Rendering::Settings::EFramebufferAttachment, uint16_t>
{
	static constexpr std::array<std::pair<AmberEditor::Rendering::Settings::EFramebufferAttachment, uint16_t>, 2> EnumMap =
	{
		std::pair{AmberEditor::Rendering::Settings::EFramebufferAttachment::COLOR, AGL_COLOR_ATTACHMENT},
		std::pair{AmberEditor::Rendering::Settings::EFramebufferAttachment::DEPTH, AGL_DEPTH_ATTACHMENT}
	};
};

template <>
struct EnumValueTypeTraits<AmberEditor::Rendering::Settings::EPixelFormat, uint16_t>
{
	static constexpr std::array<std::pair<AmberEditor::Rendering::Settings::EPixelFormat, uint16_t>, 2> EnumMap =
	{
		std::pair{AmberEditor::Rendering::Settings::EPixelFormat::RGBA8,           AGL_RGBA8},
		std::pair{AmberEditor::Rendering::Settings::EPixelFormat::DEPTH_COMPONENT, AGL_DEPTH_COMPONENT}
	};
};

template <>
struct EnumValueTypeTraits<AmberEditor::Rendering::Settings::EViewport, uint16_t>
{
	static constexpr std::array<std::pair<AmberEditor::Rendering::Settings::EViewport, uint16_t>, 1> EnumMap =
	{
		std::pair{AmberEditor::Rendering::Settings::EViewport::VIEWPORT, AGL_VIEWPORT}
	};
};

template <>
struct EnumValueTypeTraits<AmberEditor::Rendering::Settings::ETextureFilteringMode, uint16_t>
{
	static constexpr std::array<std::pair<AmberEditor::Rendering::Settings::ETextureFilteringMode, uint16_t>, 2> EnumMap =
	{
		std::pair{AmberEditor::Rendering::Settings::ETextureFilteringMode::LINEAR,  AGL_LINEAR},
		std::pair{AmberEditor::Rendering::Settings::ETextureFilteringMode::NEAREST, AGL_NEAREST}
	};
};

template <>
struct EnumValueTypeTraits<AmberEditor::Rendering::Settings::EWrapMode, uint16_t>
{
	static constexpr std::array<std::pair<AmberEditor::Rendering::Settings::EWrapMode, uint16_t>, 2> EnumMap =
	{
		std::pair{AmberEditor::Rendering::Settings::EWrapMode::CLAMP,  AGL_CLAMP},
		std::pair{AmberEditor::Rendering::Settings::EWrapMode::REPEAT, AGL_REPEAT}
	};
};

template <>
struct EnumValueTypeTraits<AmberEditor::Rendering::Settings::EUniformType, AmberGL::SoftwareRenderer::Programs::EShaderDataType>
{
	static constexpr std::array<std::pair<AmberEditor::Rendering::Settings::EUniformType, AmberGL::SoftwareRenderer::Programs::EShaderDataType>, 9> EnumMap =
	{
		std::pair{AmberEditor::Rendering::Settings::EUniformType::INT,        AmberGL::SoftwareRenderer::Programs::EShaderDataType::INT},
		std::pair{AmberEditor::Rendering::Settings::EUniformType::FLOAT,      AmberGL::SoftwareRenderer::Programs::EShaderDataType::FLOAT},
		std::pair{AmberEditor::Rendering::Settings::EUniformType::VEC2,       AmberGL::SoftwareRenderer::Programs::EShaderDataType::VEC2},
		std::pair{AmberEditor::Rendering::Settings::EUniformType::VEC3,       AmberGL::SoftwareRenderer::Programs::EShaderDataType::VEC3},
		std::pair{AmberEditor::Rendering::Settings::EUniformType::VEC4,       AmberGL::SoftwareRenderer::Programs::EShaderDataType::VEC4},
		std::pair{AmberEditor::Rendering::Settings::EUniformType::MAT2,       AmberGL::SoftwareRenderer::Programs::EShaderDataType::MAT2},
		std::pair{AmberEditor::Rendering::Settings::EUniformType::MAT3,       AmberGL::SoftwareRenderer::Programs::EShaderDataType::MAT3},
		std::pair{AmberEditor::Rendering::Settings::EUniformType::MAT4,       AmberGL::SoftwareRenderer::Programs::EShaderDataType::MAT4},
		std::pair{AmberEditor::Rendering::Settings::EUniformType::SAMPLER_2D, AmberGL::SoftwareRenderer::Programs::EShaderDataType::SAMPLER_2D},
	};
};