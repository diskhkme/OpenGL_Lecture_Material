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
	//glfw ������ ��ü�� ����� ����
	GLFWwindow *m_MainWindow;
	int m_BufferWidth, m_BufferHeight; //���� ������ ���� ũ��(������ ���۴� ���߿� ����)
	int m_Width, m_Height; //�������� ũ��
	
};