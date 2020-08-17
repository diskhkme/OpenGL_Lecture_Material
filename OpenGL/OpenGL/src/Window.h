#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"

class Window
{
public:
	Window();
	Window(int windowWidth, int windowHeight);
	~Window();

	int Initialize();

	int GetBufferWidth() { return m_BufferWidth; }
	int GetBufferHeight() { return m_BufferHeight; }

	bool GetShouldClose() { return glfwWindowShouldClose(m_MainWindow); }

	void SwapBuffers() { glfwSwapBuffers(m_MainWindow); }

private:
	//glfw 윈도우 객체를 멤버로 가짐
	GLFWwindow *m_MainWindow;
	int m_BufferWidth, m_BufferHeight; //실제 프레임 버퍼 크기(프레임 버퍼는 나중에 설명)
	int m_Width, m_Height; //윈도우의 크기
	
};