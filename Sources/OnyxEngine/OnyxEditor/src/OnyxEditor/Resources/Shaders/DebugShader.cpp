#include "OnyxEditor/Resources/Shaders/DebugShader.h"

OnyxEditor::Resources::Shaders::DebugNormalsShader::DebugNormalsShader()
{
}

glm::vec4 OnyxEditor::Resources::Shaders::DebugNormalsShader::VertexPass(const AmberGL::Geometry::Vertex& p_vertex)
{
	const glm::mat4 u_Model = GetUBOModelMatrix();
	const glm::mat4 u_View = GetUBOViewMatrix();
	const glm::mat4 u_Projection = GetUBOProjectionMatrix();

	glm::vec3 fragPos = glm::vec3(u_Model * glm::vec4(p_vertex.position, 1.0));
	SetVarying<glm::vec3>("v_FragPos", fragPos);

	glm::vec3 worldNormal = glm::mat3(glm::transpose(glm::inverse(u_Model))) * p_vertex.normal;
	SetVarying<glm::vec3>("v_Normal", worldNormal);

	return u_Projection * u_View * glm::vec4(fragPos, 1.0);
}

glm::vec4 OnyxEditor::Resources::Shaders::DebugNormalsShader::FragmentPass()
{
	const glm::vec3 normal = glm::normalize(GetVaryingAs<glm::vec3>("v_Normal"));

	return glm::vec4((normal * 0.5f) + 0.5f, 1.0f);
}

std::unique_ptr<AmberGL::SoftwareRenderer::Programs::AProgram> OnyxEditor::Resources::Shaders::DebugNormalsShader::clone() const
{
	return std::make_unique<DebugNormalsShader>(*this);
}

OnyxEditor::Resources::Shaders::DebugUVShader::DebugUVShader()
{
}

glm::vec4 OnyxEditor::Resources::Shaders::DebugUVShader::VertexPass(const AmberGL::Geometry::Vertex& p_vertex)
{
	const glm::mat4 u_Model = GetUBOModelMatrix();
	const glm::mat4 u_View = GetUBOViewMatrix();
	const glm::mat4 u_Projection = GetUBOProjectionMatrix();

	glm::vec3 fragPos = glm::vec3(u_Model * glm::vec4(p_vertex.position, 1.0));
	SetVarying<glm::vec3>("v_FragPos", fragPos);
	SetVarying<glm::vec2>("v_TexCoords", p_vertex.texCoords);

	return u_Projection * u_View * glm::vec4(fragPos, 1.0);
}

glm::vec4 OnyxEditor::Resources::Shaders::DebugUVShader::FragmentPass()
{
	const glm::vec2 texCoords = GetVaryingAs<glm::vec2>("v_TexCoords");

	return glm::vec4(texCoords.x, texCoords.y, 0.0f, 1.0f);
}

std::unique_ptr<AmberGL::SoftwareRenderer::Programs::AProgram> OnyxEditor::Resources::Shaders::DebugUVShader::clone() const
{
	return std::make_unique<DebugUVShader>(*this);
}

OnyxEditor::Resources::Shaders::DebugDepthShader::DebugDepthShader()
{
	RegisterUniform("u_zNear", 0.1f);
	RegisterUniform("u_zFar", 100.0f);
	RegisterUniform("colorImage", 0);
}

glm::vec4 OnyxEditor::Resources::Shaders::DebugDepthShader::VertexPass(const AmberGL::Geometry::Vertex& p_vertex)
{
	SetVarying<glm::vec2>("texCoords", p_vertex.texCoords);

	return glm::vec4(p_vertex.position, 1.0f);
}

float LinearizeDepth(float p_depth)
{
	float zNear = 0.5f;
	float zFar = 100.0f;

	return (2.0f * zNear) / (zFar + zNear - p_depth * (zFar - zNear));
}

glm::vec4 OnyxEditor::Resources::Shaders::DebugDepthShader::FragmentPass()
{
	const glm::vec2 texCoords = GetVaryingAs<glm::vec2>("texCoords");

	// auto depthTexture = GetSample("u_DepthMap");
	float depth = Texture("colorImage", texCoords).x;
	//float depth = Texture(*depthTexture, texCoords).x;
	float test = LinearizeDepth(depth);
	return glm::vec4(test, test, test, 1.0f);
}

std::unique_ptr<AmberGL::SoftwareRenderer::Programs::AProgram> OnyxEditor::Resources::Shaders::DebugDepthShader::clone() const
{
	return std::make_unique<DebugDepthShader>(*this);
}

OnyxEditor::Resources::Shaders::DebugCheckerboardShader::DebugCheckerboardShader()
{
}

glm::vec4 OnyxEditor::Resources::Shaders::DebugCheckerboardShader::VertexPass(const AmberGL::Geometry::Vertex& p_vertex)
{
	const glm::mat4 u_Model = GetUBOModelMatrix();
	const glm::mat4 u_View = GetUBOViewMatrix();
	const glm::mat4 u_Projection = GetUBOProjectionMatrix();

	glm::vec3 fragPos = glm::vec3(u_Model * glm::vec4(p_vertex.position, 1.0));
	SetVarying<glm::vec3>("v_FragPos", fragPos);
	SetVarying<glm::vec2>("v_TexCoords", p_vertex.texCoords);

	return u_Projection * u_View * glm::vec4(fragPos, 1.0);
}

glm::vec4 OnyxEditor::Resources::Shaders::DebugCheckerboardShader::FragmentPass()
{
	const glm::vec2 texCoords = GetVaryingAs<glm::vec2>("v_TexCoords");

	float checkerSize = 8.0f;
	glm::vec2 checker = glm::floor(texCoords * checkerSize);
	float checkerValue = fmod(checker.x + checker.y, 2.0f);

	glm::vec3 color1 = glm::vec3(0.8f, 0.8f, 0.8f);
	glm::vec3 color2 = glm::vec3(0.2f, 0.2f, 0.2f);

	glm::vec3 finalColor = glm::mix(color1, color2, checkerValue);

	return glm::vec4(finalColor, 1.0f);
}

std::unique_ptr<AmberGL::SoftwareRenderer::Programs::AProgram> OnyxEditor::Resources::Shaders::DebugCheckerboardShader::clone() const
{
	return std::make_unique<DebugCheckerboardShader>(*this);
}
