#include "OnyxEditor/Rendering/RenderState.h"

OnyxEditor::Rendering::RenderState::RenderState() :
CullFace(Settings::ECullFace::BACK),
PolygonMode(Settings::EPolygonMode::FILL),
DepthFunc(Settings::EComparisonOperand::LESS),
LineWidth(1.0f),
PointSize(1.0f),
MultiSample(true),
DepthTest(true),
DepthWriting(true),
ColorWriting(true),
Blending(false),
Culling(true),
StencilTest(false),
StencilFunc(Settings::EComparisonOperand::ALWAYS),
StencilRef(0),
StencilMask(0xFFFFFFFF),
StencilWriteMask(0xFFFFFFFF),
StencilFail(EStencilOp::KEEP),
StencilPassDepthFail(EStencilOp::KEEP),
StencilPassDepthPass(EStencilOp::KEEP)
{
}
