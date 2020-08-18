#include "Renderer.h"
#include "Shader.h"

#include <iostream>

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR); // GL_NO_ERROR == 0
}

bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << ") : " << function <<
			" " << file << " in line " << line << std::endl;
		return false;
	}
	return true;
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	//�׸��� ���� ������ �׸��� ���ε�
	shader.Bind();
	va.Bind();
	ib.Bind();

	//�׸���
	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
