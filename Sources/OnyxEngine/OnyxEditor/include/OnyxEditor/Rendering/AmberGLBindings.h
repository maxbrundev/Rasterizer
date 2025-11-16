#pragma once

#include <array>

#include <AmberGL/SoftwareRenderer/Defines.h>
#include <AmberGL/SoftwareRenderer/Programs/EShaderUniformType.h>

#include "OnyxEditor/Rendering/Settings/EBufferType.h"
#include "OnyxEditor/Rendering/Settings/EClearFlag.h"
#include "OnyxEditor/Rendering/Settings/EComparisonOperand.h"
#include "OnyxEditor/Rendering/Settings/ECullFace.h"
#include "OnyxEditor/Rendering/Settings/EFilterMode.h"
#include "OnyxEditor/Rendering/Settings/EFramebufferAttachment.h"
#include "OnyxEditor/Rendering/Settings/EPixelFormat.h"
#include "OnyxEditor/Rendering/Settings/EPolygonMode.h"
#include "OnyxEditor/Rendering/Settings/EPrimitiveMode.h"
#include "OnyxEditor/Rendering/Settings/ERenderingCapability.h"
#include "OnyxEditor/Rendering/Settings/ETextureFilteringMode.h"
#include "OnyxEditor/Rendering/Settings/EUniformType.h"
#include "OnyxEditor/Rendering/Settings/EViewport.h"
#include "OnyxEditor/Rendering/Settings/EWrapMode.h"

template<typename Enum, typename Value>
struct EnumValueTypeTraits
{
};

template <>
struct EnumValueTypeTraits<OnyxEditor::Rendering::Settings::EClearFlag, uint16_t>
{
	static constexpr std::array<std::pair<OnyxEditor::Rendering::Settings::EClearFlag, uint16_t>, 3> EnumMap =
	{
		std::pair{OnyxEditor::Rendering::Settings::EClearFlag::COLOR, AGL_COLOR_BUFFER_BIT},
		std::pair{OnyxEditor::Rendering::Settings::EClearFlag::DEPTH, AGL_DEPTH_BUFFER_BIT},
		std::pair{OnyxEditor::Rendering::Settings::EClearFlag::STENCIL, AGL_STENCIL_BUFFER_BIT}
	};
};

template <>
struct EnumValueTypeTraits<OnyxEditor::Rendering::Settings::EPolygonMode, uint16_t>
{
	static constexpr std::array<std::pair<OnyxEditor::Rendering::Settings::EPolygonMode, uint16_t>, 3> EnumMap =
	{
		std::pair{OnyxEditor::Rendering::Settings::EPolygonMode::FILL,  AGL_FILL},
		std::pair{OnyxEditor::Rendering::Settings::EPolygonMode::LINE,  AGL_LINE},
		std::pair{OnyxEditor::Rendering::Settings::EPolygonMode::POINT, AGL_POINT}
	};
};

template <>
struct EnumValueTypeTraits<OnyxEditor::Rendering::Settings::EPrimitiveMode, uint16_t>
{
	static constexpr std::array<std::pair<OnyxEditor::Rendering::Settings::EPrimitiveMode, uint16_t>, 4> EnumMap =
	{
		std::pair{OnyxEditor::Rendering::Settings::EPrimitiveMode::LINES,          AGL_LINES},
		std::pair{OnyxEditor::Rendering::Settings::EPrimitiveMode::POINTS,         AGL_POINTS},
		std::pair{OnyxEditor::Rendering::Settings::EPrimitiveMode::TRIANGLES,      AGL_TRIANGLES},
		std::pair{OnyxEditor::Rendering::Settings::EPrimitiveMode::TRIANGLE_STRIP, AGL_TRIANGLE_STRIP}
	};
};

template <>
struct EnumValueTypeTraits<OnyxEditor::Rendering::Settings::ECullFace, uint16_t>
{
	static constexpr std::array<std::pair<OnyxEditor::Rendering::Settings::ECullFace, uint16_t>, 3> EnumMap =
	{
		std::pair{OnyxEditor::Rendering::Settings::ECullFace::BACK,           AGL_BACK},
		std::pair{OnyxEditor::Rendering::Settings::ECullFace::FRONT,          AGL_FRONT},
		std::pair{OnyxEditor::Rendering::Settings::ECullFace::FRONT_AND_BACK, AGL_FRONT_AND_BACK}
	};
};

template <>
struct EnumValueTypeTraits<OnyxEditor::Rendering::Settings::ERenderingCapability, uint8_t>
{
	static constexpr std::array<std::pair<OnyxEditor::Rendering::Settings::ERenderingCapability, uint8_t>, 5> EnumMap =
	{
		std::pair{OnyxEditor::Rendering::Settings::ERenderingCapability::DEPTH_WRITE,  AGL_DEPTH_WRITE},
		std::pair{OnyxEditor::Rendering::Settings::ERenderingCapability::DEPTH_TEST,   AGL_DEPTH_TEST},
		std::pair{OnyxEditor::Rendering::Settings::ERenderingCapability::CULL_FACE,    AGL_CULL_FACE},
		std::pair{OnyxEditor::Rendering::Settings::ERenderingCapability::MULTISAMPLE,  AGL_MULTISAMPLE},
		std::pair{OnyxEditor::Rendering::Settings::ERenderingCapability::STENCIL_TEST, AGL_STENCIL_TEST}
	};
};

template <>
struct EnumValueTypeTraits<OnyxEditor::Rendering::Settings::EBufferType, uint16_t>
{
	static constexpr std::array<std::pair<OnyxEditor::Rendering::Settings::EBufferType, uint16_t>, 2> EnumMap =
	{
		std::pair{OnyxEditor::Rendering::Settings::EBufferType::VERTEX, AGL_ARRAY_BUFFER},
		std::pair{OnyxEditor::Rendering::Settings::EBufferType::INDEX,  AGL_ELEMENT_ARRAY_BUFFER}
	};
};

template <>
struct EnumValueTypeTraits<OnyxEditor::Rendering::Settings::EComparisonOperand, uint16_t>
{
	static constexpr std::array<std::pair<OnyxEditor::Rendering::Settings::EComparisonOperand, uint16_t>, 8> EnumMap =
	{
		std::pair{OnyxEditor::Rendering::Settings::EComparisonOperand::NEVER,         AGL_NEVER},
		std::pair{OnyxEditor::Rendering::Settings::EComparisonOperand::LESS,          AGL_LESS},
		std::pair{OnyxEditor::Rendering::Settings::EComparisonOperand::EQUAL,         AGL_EQUAL},
		std::pair{OnyxEditor::Rendering::Settings::EComparisonOperand::LESS_EQUAL,    AGL_LEQUAL},
		std::pair{OnyxEditor::Rendering::Settings::EComparisonOperand::GREATER,       AGL_GREATER},
		std::pair{OnyxEditor::Rendering::Settings::EComparisonOperand::NOT_EQUAL,     AGL_NOTEQUAL},
		std::pair{OnyxEditor::Rendering::Settings::EComparisonOperand::GREATER_EQUAL, AGL_GEQUAL},
		std::pair{OnyxEditor::Rendering::Settings::EComparisonOperand::ALWAYS,        AGL_ALWAYS}
	};
};

template <>
struct EnumValueTypeTraits<OnyxEditor::Rendering::Settings::EFilterMode, uint16_t>
{
	static constexpr std::array<std::pair<OnyxEditor::Rendering::Settings::EFilterMode, uint16_t>, 2> EnumMap =
	{
		std::pair{OnyxEditor::Rendering::Settings::EFilterMode::LINEAR,  AGL_LINEAR},
		std::pair{OnyxEditor::Rendering::Settings::EFilterMode::NEAREST, AGL_NEAREST}
	};
};

template <>
struct EnumValueTypeTraits<OnyxEditor::Rendering::Settings::EFramebufferAttachment, uint16_t>
{
	static constexpr std::array<std::pair<OnyxEditor::Rendering::Settings::EFramebufferAttachment, uint16_t>, 2> EnumMap =
	{
		std::pair{OnyxEditor::Rendering::Settings::EFramebufferAttachment::COLOR, AGL_COLOR_ATTACHMENT},
		std::pair{OnyxEditor::Rendering::Settings::EFramebufferAttachment::DEPTH, AGL_DEPTH_ATTACHMENT}
	};
};

template <>
struct EnumValueTypeTraits<OnyxEditor::Rendering::Settings::EPixelFormat, uint16_t>
{
	static constexpr std::array<std::pair<OnyxEditor::Rendering::Settings::EPixelFormat, uint16_t>, 2> EnumMap =
	{
		std::pair{OnyxEditor::Rendering::Settings::EPixelFormat::RGBA8,           AGL_RGBA8},
		std::pair{OnyxEditor::Rendering::Settings::EPixelFormat::DEPTH_COMPONENT, AGL_DEPTH_COMPONENT}
	};
};

template <>
struct EnumValueTypeTraits<OnyxEditor::Rendering::Settings::EViewport, uint16_t>
{
	static constexpr std::array<std::pair<OnyxEditor::Rendering::Settings::EViewport, uint16_t>, 1> EnumMap =
	{
		std::pair{OnyxEditor::Rendering::Settings::EViewport::VIEWPORT, AGL_VIEWPORT}
	};
};

template <>
struct EnumValueTypeTraits<OnyxEditor::Rendering::Settings::ETextureFilteringMode, uint16_t>
{
	static constexpr std::array<std::pair<OnyxEditor::Rendering::Settings::ETextureFilteringMode, uint16_t>, 2> EnumMap =
	{
		std::pair{OnyxEditor::Rendering::Settings::ETextureFilteringMode::LINEAR,  AGL_LINEAR},
		std::pair{OnyxEditor::Rendering::Settings::ETextureFilteringMode::NEAREST, AGL_NEAREST}
	};
};

template <>
struct EnumValueTypeTraits<OnyxEditor::Rendering::Settings::EWrapMode, uint16_t>
{
	static constexpr std::array<std::pair<OnyxEditor::Rendering::Settings::EWrapMode, uint16_t>, 2> EnumMap =
	{
		std::pair{OnyxEditor::Rendering::Settings::EWrapMode::CLAMP,  AGL_CLAMP},
		std::pair{OnyxEditor::Rendering::Settings::EWrapMode::REPEAT, AGL_REPEAT}
	};
};

template <>
struct EnumValueTypeTraits<OnyxEditor::Rendering::Settings::EUniformType, AmberGL::SoftwareRenderer::Programs::EShaderDataType>
{
	static constexpr std::array<std::pair<OnyxEditor::Rendering::Settings::EUniformType, AmberGL::SoftwareRenderer::Programs::EShaderDataType>, 9> EnumMap =
	{
		std::pair{OnyxEditor::Rendering::Settings::EUniformType::INT,        AmberGL::SoftwareRenderer::Programs::EShaderDataType::INT},
		std::pair{OnyxEditor::Rendering::Settings::EUniformType::FLOAT,      AmberGL::SoftwareRenderer::Programs::EShaderDataType::FLOAT},
		std::pair{OnyxEditor::Rendering::Settings::EUniformType::VEC2,       AmberGL::SoftwareRenderer::Programs::EShaderDataType::VEC2},
		std::pair{OnyxEditor::Rendering::Settings::EUniformType::VEC3,       AmberGL::SoftwareRenderer::Programs::EShaderDataType::VEC3},
		std::pair{OnyxEditor::Rendering::Settings::EUniformType::VEC4,       AmberGL::SoftwareRenderer::Programs::EShaderDataType::VEC4},
		std::pair{OnyxEditor::Rendering::Settings::EUniformType::MAT2,       AmberGL::SoftwareRenderer::Programs::EShaderDataType::MAT2},
		std::pair{OnyxEditor::Rendering::Settings::EUniformType::MAT3,       AmberGL::SoftwareRenderer::Programs::EShaderDataType::MAT3},
		std::pair{OnyxEditor::Rendering::Settings::EUniformType::MAT4,       AmberGL::SoftwareRenderer::Programs::EShaderDataType::MAT4},
		std::pair{OnyxEditor::Rendering::Settings::EUniformType::SAMPLER_2D, AmberGL::SoftwareRenderer::Programs::EShaderDataType::SAMPLER_2D},
	};
};