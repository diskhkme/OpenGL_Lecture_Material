#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"


VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID)); //vao 생성
	//glBindVertexArray(m_RendererID); //vao 바인딩(="작업 상태") <-- 바인딩은 AddBuffer 직전에 수행하도록 함
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer & vb, const VertexBufferLayout & layout)
{
	Bind(); //vao를 바인딩

	vb.Bind(); //Vertex Buffer를 바인딩
	
	const auto& elements = layout.GetElement();
	unsigned int offset = 0;
	for (int i=0;i<elements.size();i++)
	{
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i)); //기존에는 0번만 존재했으나, position/normal/color등 여러 attribute가 생기면, 여러 attribute를 enable해야함
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset)); //layout별로 데이터를 어떻게 읽어와야하는지를 element구조체로 가지고 있을 예정. 이를 활용함.
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