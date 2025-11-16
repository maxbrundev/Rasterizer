#pragma once

#include <AmberGL/SoftwareRenderer/Programs/AProgram.h>

namespace OnyxEditor::Resources::Shaders
{
	class DebugNormalsShader : public AmberGL::SoftwareRenderer::Programs::AProgram
	{
	public:
		DebugNormalsShader();
		~DebugNormalsShader() override = default;

	protected:
		glm::vec4 VertexPass(const AmberGL::Geometry::Vertex& p_vertex) override;
		glm::vec4 FragmentPass() override;

	public:
		std::unique_ptr<AProgram> clone() const override;
	};

	class DebugUVShader : public AmberGL::SoftwareRenderer::Programs::AProgram
	{
	public:
		DebugUVShader();
		~DebugUVShader() override = default;

	protected:
		glm::vec4 VertexPass(const AmberGL::Geometry::Vertex& p_vertex) override;
		glm::vec4 FragmentPass() override;

	public:
		std::unique_ptr<AProgram> clone() const override;
	};

	class DebugDepthShader : public AmberGL::SoftwareRenderer::Programs::AProgram
	{
	public:
		DebugDepthShader();
		~DebugDepthShader() override = default;

	protected:
		glm::vec4 VertexPass(const AmberGL::Geometry::Vertex& p_vertex) override;
		glm::vec4 FragmentPass() override;

	public:
		std::unique_ptr<AProgram> clone() const override;
	};

	class DebugCheckerboardShader : public AmberGL::SoftwareRenderer::Programs::AProgram
	{
	public:
		DebugCheckerboardShader();
		~DebugCheckerboardShader() override = default;

	protected:
		glm::vec4 VertexPass(const AmberGL::Geometry::Vertex& p_vertex) override;
		glm::vec4 FragmentPass() override;

	public:
		std::unique_ptr<AProgram> clone() const override;
	};
}
