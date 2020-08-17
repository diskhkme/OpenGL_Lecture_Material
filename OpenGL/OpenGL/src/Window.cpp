#include "Window.h"

#include <iostream>

Window::Window()
	:m_Width{ 800 }, m_Height{ 600 }
{
}

Window::Window(int windowWidth, int windowHeight)
	: m_Width{ windowWidth }, m_Height{ windowHeight }
{
}

Window::~Window()
{
	glfwDestroyWindow(m_MainWindow);
	glfwTerminate();
}

int Window::Initialize()
{
	/* Initialize the library */
	if (!glfwInit())
		return -1;
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Opengl 3.3 ���� ���
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	m_MainWindow = glfwCreateWindow(m_Width, m_Height, "Hello World", NULL, NULL);
	if (!m_MainWindow)
	{
		glfwTerminate();
		return -1;
	}

	//�߰��� �ڵ�, ���� ���� ũ�⸦ ������(����� ������ ũ��� ������, ��Ȳ�� ���� �ٸ� �� ����)
	glfwGetFramebufferSize(m_MainWindow, &m_BufferWidth, &m_BufferHeight);

	/* Make the window's context current */
	glfwMakeContextCurrent(m_MainWindow);

	glfwSwapInterval(1); //1�̸� vsync rate�� ���� �ӵ��� ȭ�� ����

		// glfwMakeContextCurrent�� ȣ��� �Ŀ� glewInit�� ����Ǿ�� ��
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error\n";
	}

	glViewport(0, 0, m_BufferWidth, m_BufferHeight);

	std::cout << glGetString(GL_VERSION) << std::endl; //�� �÷����� GL_Version ����غ���

	//���� ä�� ó�� ��� (chapter 10���� �ٷ� ����)
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}
