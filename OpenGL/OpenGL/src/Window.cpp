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
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Opengl 3.3 버전 사용
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	m_MainWindow = glfwCreateWindow(m_Width, m_Height, "Hello World", NULL, NULL);
	if (!m_MainWindow)
	{
		glfwTerminate();
		return -1;
	}

	//추가된 코드, 실제 버퍼 크기를 가져옴(현재는 윈도우 크기와 같지만, 상황에 따라 다를 수 있음)
	glfwGetFramebufferSize(m_MainWindow, &m_BufferWidth, &m_BufferHeight);

	/* Make the window's context current */
	glfwMakeContextCurrent(m_MainWindow);
		
	glfwSwapInterval(1); //1이면 vsync rate와 같은 속도로 화면 갱신

	// glfwMakeContextCurrent가 호출된 후에 glewInit이 수행되어야 함
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error\n";
	}

	glViewport(0, 0, m_BufferWidth, m_BufferHeight);
	
	std::cout << glGetString(GL_VERSION) << std::endl; //내 플랫폼의 GL_Version 출력해보기
	

	//콜백 등록
	CreateCallback();
	glfwSetInputMode(m_MainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//프로그램 실행 도중 마우스 커서가 보이지 않도록 설정하는 옵션

	//callback은 static 함수로 선언되었으므로, GLFW에게 현재 GLFW윈도우를 포함하는 내가 만든 윈도우 객체 포인터를 알림
	//https://stackoverflow.com/questions/55145966/what-does-glfwgetwindowuserpointer-do
	glfwSetWindowUserPointer(m_MainWindow, this);

	//알파 채널 처리 방법 (chapter 10에서 다룰 예정)
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
	m_XChange = 0.0f; //매 프레임마다 변화량은 0으로 새로 설정
	return theChange;
}

float Window::GetYChange()
{
	float theChange = m_YChange;
	m_YChange = 0.0f; //매 프레임마다 변화량은 0으로 새로 설정
	return theChange;
}

void Window::HandleKeys(GLFWwindow * window, int key, int code, int action, int mode)
	//key: 어떤 키인지, code: 플랫폼 특정 scan code(사용안함)
	//action: 키에서 발생한 행동(눌림, 뗌 등), mode: 컨트롤, 알트, 쉬프트 등 입력
{
	//72 line에서 저장한 객체 포인터를 반환
	Window *windowObject = static_cast<Window*>(glfwGetWindowUserPointer(window));

	//ESC 키가 눌리면 윈도우 닫기
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			//특정 키가 눌렸으면, m_Keys 배열의 해당 키에 해당하는 값을 true로 설정
			windowObject->m_Keys[key] = true;
			//std::cout << "Pressed : " << key << std::endl;
		}
		else if (action == GLFW_RELEASE)
		{
			//특정 키가 떼졌으면, m_Keys 배열의 해당 키에 해당하는 값을 false로 설정
			windowObject->m_Keys[key] = false;
			//std::cout << "Release : " << key << std::endl;
		}
	}
}

void Window::HandleMouse(GLFWwindow * window, double xPos, double yPos)
	//xpos, ypos : 마우스의 윈도우 내에서의 x,y 좌표
{
	//72 line에서 저장한 객체 포인터를 반환
	Window *windowObject = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (windowObject->m_MouseFirstMoved) //마우스가 처음 움직이기 시작했을 때의 좌표를 저장해둠
	{
		windowObject->m_LastX = xPos;
		windowObject->m_LastY = yPos;
		windowObject->m_MouseFirstMoved = false;
	}
	else //계속 움직인다면
	{
		//이전 프레임 위치에서 현재 프레임 위치를 뺀 값을 Change에 저장
		//y좌표는 뒤집어줌 (또는 여기서 뒤집지 않고 구현 로직에서 처리해도 상관 없음)
		windowObject->m_XChange = xPos - windowObject->m_LastX;
		windowObject->m_YChange = windowObject->m_LastY - yPos;

		//변화량을 계산했으므로, 최신 위치를 last로 저장
		windowObject->m_LastX = xPos;
		windowObject->m_LastY = yPos;

		/*std::cout << "XChange : " << windowObject->m_XChange <<
			" YChange : " << windowObject->m_YChange << std::endl;*/
	}

}

void Window::CreateCallback()
{
	//HandleKeys 함수와 HandleMouse 함수를 GLFW 윈도우의 키보드/마우스 입력 콜백 함수로 정의
	glfwSetKeyCallback(m_MainWindow, HandleKeys);
	glfwSetCursorPosCallback(m_MainWindow, HandleMouse);
}
