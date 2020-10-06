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

	//Ű����/���콺 �Է� ó���� ���� ����Լ�
	bool* GetKeys() { return m_Keys; }
	float GetXChange();
	float GetYChange();
	void ChangeViewPort(int width, int height);
private:
	//glfw ������ ��ü�� ����� ����
	GLFWwindow *m_MainWindow;
	int m_BufferWidth, m_BufferHeight; //���� ������ ���� ũ��(������ ���۴� ���߿� ����)
	int m_Width, m_Height; //�������� ũ��

	//Ű����/���콺 �Է� ó���� ���� �������
	bool m_Keys[1024]; //Ű���尡 ���ȴ��� ���θ� ������ �迭
	float m_LastX, m_LastY; //���� �������� ���콺 X,Y��ġ ����
	float m_XChange, m_YChange; //�̹� �����ӿ��� ���� �����ӱ��� ���콺 �̵��� dX, dY ����
	bool m_MouseFirstMoved; //���콺�� ó�� �����̱� �����ߴ��� ����

	//GLFW �����쿡 ����� �̺�Ʈ �ݹ�
	//https://www.glfw.org/docs/latest/input_guide.html#input_keyboard
	//�ݹ��� GLFWWindow�� ����ϸ�, Ű���� �� ���콺 �Է��� �߻��� ������ �Լ����� ȣ���
	static void HandleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void HandleMouse(GLFWwindow* window, double xPos, double yPos);

	void CreateCallback(); //�ݹ� ���
};