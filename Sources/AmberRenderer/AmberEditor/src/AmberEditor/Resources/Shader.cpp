#include "AmberEditor/Resources/Shader.h"

#include <AmberGL/SoftwareRenderer/AmberGL.h>

AmberEditor::Resources::Shader::Shader(const std::string& p_name, uint32_t p_id, AmberGL::SoftwareRenderer::Programs::AProgram* p_program) :
Name(p_name),
ID(p_id),
m_program(p_program)
{
}

void AmberEditor::Resources::Shader::Bind() const
{
	AmberGL::UseProgram(ID);
}

void AmberEditor::Resources::Shader::Unbind() const
{
	uint32_t programId = 0;
	AmberGL::UseProgram(programId);
}

AmberGL::SoftwareRenderer::Programs::AProgram* AmberEditor::Resources::Shader::GetProgram() const
{
	return m_program;
}
