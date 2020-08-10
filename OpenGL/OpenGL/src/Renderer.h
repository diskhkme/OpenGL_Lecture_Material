#pragma once

#include <GL/glew.h>

#include "VertexArray.h"
#include "IndexBuffer.h"

class Shader; //include 순환 문제..

#define ASSERT(x) if ((!x)) __debugbreak(); 
#define GLCall(x) GLClearError();\
				  x;\
				  ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public:
	//화면에 무언가를 그리위해서는 Vertex Array (Vertex Buffer는 여기에 포함되어 있음), Index Array, Shader Program이 필요함
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const; 
	void Clear() const;
};