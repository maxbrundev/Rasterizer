#include "AmberGL/SoftwareRenderer/Programs/QuadNDC.h"

glm::vec4 AmberGL::SoftwareRenderer::Programs::QuadNDC::VertexPass(const Geometry::Vertex& p_vertex)
{
	SetVarying<glm::vec2>("v_TexCoords", p_vertex.texCoords);

	return glm::vec4(p_vertex.position, 1.0);
}

float LinearizeDepth(float p_depth, float p_nearPlane, float p_farPlane)
{
	float z = p_depth * 2.0f - 1.0f;
	return (2.0 * p_nearPlane * p_farPlane) / (p_farPlane + p_nearPlane - z * (p_farPlane - p_nearPlane));
}

glm::vec4 AmberGL::SoftwareRenderer::Programs::QuadNDC::FragmentPass()
{
	const glm::vec2 texCoords = GetVaryingAs<glm::vec2>("v_TexCoords");

	// auto depthTexture = GetSample("u_DepthMap");
	float depth = Texture("shadowMap", texCoords).x;
	//float depth = Texture(*depthTexture, texCoords).x;

	return glm::vec4(depth, depth, depth, 1.0f);
}
