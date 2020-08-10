#pragma once

#include <vector>

#include "Renderer.h"

//Layout����, �����͸� ��� �о�;� �ϴ����� ���� ������ �������ִ� ����ü
struct VertexBufferElement
{
	unsigned int type; //�� ������ Ÿ���� �������� (ex, vertex�� ��ġ�� float)
	unsigned int count; //�����Ͱ� �� ������
	unsigned char normalized; //�������� normalization�� �ʿ�����

	static unsigned int GetSizeOfType(unsigned int type) //Ÿ�Ժ��� ������ �޸� ����� ��ȯ�ϴ� static �Լ�
	{
		switch (type)
		{
			case GL_FLOAT: return 4;
			case GL_UNSIGNED_INT: return 4;
			case GL_UNSIGNED_BYTE: return 1;
		}
		ASSERT(0);
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements; //�ϳ��� layout�� �������� element�� ���� ����(ex, position, normal, color, etc...)
	unsigned int m_Stride; //vertex�ϳ��� �����Ͱ� �󸶳� �������ִ��� stride�� ��������� ���� ����

public:
	VertexBufferLayout()
		: m_Stride{ 0 }
	{}

	template<typename T>
	void Push(unsigned int count)
	{
		static_assert(false);
	}

	//template specializations
	template<>
	void Push<float>(unsigned int count)
	{
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT); //vertex �ϳ��� float �����Ͱ� count�� �߰��ɼ���, count * size(GL_FLOAT)�� stride�� Ŀ���� ��
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT); //���� ��������
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferElement>& GetElement() const { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }
};