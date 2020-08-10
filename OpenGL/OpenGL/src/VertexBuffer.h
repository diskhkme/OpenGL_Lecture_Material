#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID;
public:
	VertexBuffer(const void* data, unsigned int size); //size�� byte ������, �������� Ÿ���� �𸣱� ������ void*��.
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};