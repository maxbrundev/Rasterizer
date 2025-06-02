#include "AmberEditor/Rendering/RenderState.h"

AmberEditor::Rendering::RenderState::RenderState() :
CullFace(Settings::ECullFace::BACK),
PolygonMode(Settings::EPolygonMode::FILL),
DepthFunc(Settings::EComparisonOperand::LESS),
LineWidth(1.0f),
PointSize(1.0f),
MultiSample(true),
DepthTest(true),
DepthWriting(true),
Blending(false),
Culling(true)
{
}
