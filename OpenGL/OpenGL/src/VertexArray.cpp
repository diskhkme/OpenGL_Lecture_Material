#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"


VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID)); //vao ����
	//glBindVertexArray(m_RendererID); //vao ���ε�(="�۾� ����") <-- ���ε��� AddBuffer ������ �����ϵ��� ��
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer & vb, const VertexBufferLayout & layout)
{
	Bind(); //vao�� ���ε�

	vb.Bind(); //Vertex Buffer�� ���ε�
	
	const auto& elements = layout.GetElement();
	unsigned int offset = 0;
	for (int i=0;i<elements.size();i++)
	{
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i)); //�������� 0���� ����������, position/normal/color�� ���� attribute�� �����, ���� attribute�� enable�ؾ���
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset)); //layout���� �����͸� ��� �о�;��ϴ����� element����ü�� ������ ���� ����. �̸� Ȱ����.
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
	
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}