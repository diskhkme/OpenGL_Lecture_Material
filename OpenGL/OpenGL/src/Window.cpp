#include "Window.h"

#include <iostream>

Window::Window()
	:m_Width{ 800 }, m_Height{ 600 }, m_MouseFirstMoved{ true }
{
	for (int i = 0; i < 1024; i++)
	{
		m_Keys[i] = false;
	}
}

Window::Window(int windowWidth, int windowHeight)
	: m_Width{ windowWidth }, m_Height{ windowHeight }, m_MouseFirstMoved{ true }
{
	for (int i = 0; i < 1024; i++)
	{
		m_Keys[i] = false;
	}
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


	//�ݹ� ���
	CreateCallback();
	glfwSetInputMode(m_MainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//���α׷� ���� ���� ���콺 Ŀ���� ������ �ʵ��� �����ϴ� �ɼ�

	//callback�� static �Լ��� ����Ǿ����Ƿ�, GLFW���� ���� GLFW�����츦 �����ϴ� ���� ���� ������ ��ü �����͸� �˸�
	//https://stackoverflow.com/questions/55145966/what-does-glfwgetwindowuserpointer-do
	glfwSetWindowUserPointer(m_MainWindow, this);

	//���� ä�� ó�� ��� (chapter 10���� �ٷ� ����)
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	//cull face (chapter 7, back face culling)
	GLCall(glEnable(GL_CULL_FACE));
	GLCall(glCullFace(GL_BACK));

	//depth test(chapter 10, z-buffering)
	GLCall(glEnable(GL_DEPTH_TEST));
}

float Window::GetXChange()
{
	float theChange = m_XChange;
	m_XChange = 0.0f; //�� �����Ӹ��� ��ȭ���� 0���� ���� ����
	return theChange;
}

float Window::GetYChange()
{
	float theChange = m_YChange;
	m_YChange = 0.0f; //�� �����Ӹ��� ��ȭ���� 0���� ���� ����
	return theChange;
}

void Window::ChangeViewPort(int width, int height)
{
	GLCall(glViewport(0, 0, width, height));
}

void Window::HandleKeys(GLFWwindow * window, int key, int code, int action, int mode)
//key: � Ű����, code: �÷��� Ư�� scan code(������)
//action: Ű���� �߻��� �ൿ(����, �� ��), mode: ��Ʈ��, ��Ʈ, ����Ʈ �� �Է�
{
	//72 line���� ������ ��ü �����͸� ��ȯ
	Window *windowObject = static_cast<Window*>(glfwGetWindowUserPointer(window));

	//ESC Ű�� ������ ������ �ݱ�
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			//Ư�� Ű�� ��������, m_Keys �迭�� �ش� Ű�� �ش��ϴ� ���� true�� ����
			windowObject->m_Keys[key] = true;
			//std::cout << "Pressed : " << key << std::endl;
		}
		else if (action == GLFW_RELEASE)
		{
			//Ư�� Ű�� ��������, m_Keys �迭�� �ش� Ű�� �ش��ϴ� ���� false�� ����
			windowObject->m_Keys[key] = false;
			//std::cout << "Release : " << key << std::endl;
		}
	}
}

void Window::HandleMouse(GLFWwindow * window, double xPos, double yPos)
//xpos, ypos : ���콺�� ������ �������� x,y ��ǥ
{
	//72 line���� ������ ��ü �����͸� ��ȯ
	Window *windowObject = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (windowObject->m_MouseFirstMoved) //���콺�� ó�� �����̱� �������� ���� ��ǥ�� �����ص�
	{
		windowObject->m_LastX = xPos;
		windowObject->m_LastY = yPos;
		windowObject->m_MouseFirstMoved = false;
	}
	else //��� �����δٸ�
	{
		//���� ������ ��ġ���� ���� ������ ��ġ�� �� ���� Change�� ����
		//y��ǥ�� �������� (�Ǵ� ���⼭ ������ �ʰ� ���� �������� ó���ص� ��� ����)
		windowObject->m_XChange = xPos - windowObject->m_LastX;
		windowObject->m_YChange = windowObject->m_LastY - yPos;

		//��ȭ���� ��������Ƿ�, �ֽ� ��ġ�� last�� ����
		windowObject->m_LastX = xPos;
		windowObject->m_LastY = yPos;

		/*std::cout << "XChange : " << windowObject->m_XChange <<
			" YChange : " << windowObject->m_YChange << std::endl;*/
	}

}

void Window::CreateCallback()
{
	//HandleKeys �Լ��� HandleMouse �Լ��� GLFW �������� Ű����/���콺 �Է� �ݹ� �Լ��� ����
	glfwSetKeyCallback(m_MainWindow, HandleKeys);
	glfwSetCursorPosCallback(m_MainWindow, HandleMouse);
}