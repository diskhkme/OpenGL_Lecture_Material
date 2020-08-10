#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID;
public:
	VertexBuffer(const void* data, unsigned int size); //size는 byte 사이즈, 데이터의 타입은 모르기 때문에 void*로.
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};