#include "AmberRenderer/Resources/Shader.h"

#include "AmberRenderer/Rendering/SoftwareRenderer/AmberGL.h"

AmberRenderer::Resources::Shader::Shader(const std::string& p_name, uint32_t p_id, Rendering::SoftwareRenderer::Programs::AProgram* p_program) :
Name(p_name),
ID(p_id),
m_program(p_program)
{
}

void AmberRenderer::Resources::Shader::Bind() const
{
	AmberGL::UseProgram(ID);
}

void AmberRenderer::Resources::Shader::Unbind() const
{
	uint32_t programId = 0;
	AmberGL::UseProgram(programId);
}

AmberRenderer::Rendering::SoftwareRenderer::Programs::AProgram* AmberRenderer::Resources::Shader::GetProgram() const
{
	return m_program;
}
