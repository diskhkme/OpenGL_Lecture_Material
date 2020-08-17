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

	//키보드/마우스 입력 처리를 위한 멤버함수
	bool* GetKeys() { return m_Keys; }
	float GetXChange();
	float GetYChange();

private:
	//glfw 윈도우 객체를 멤버로 가짐
	GLFWwindow *m_MainWindow;
	int m_BufferWidth, m_BufferHeight; //실제 프레임 버퍼 크기(프레임 버퍼는 나중에 설명)
	int m_Width, m_Height; //윈도우의 크기
	
	//키보드/마우스 입력 처리를 위한 멤버변수
	bool m_Keys[1024]; //키보드가 눌렸는지 여부를 저장할 배열
	float m_LastX, m_LastY; //이전 프레임의 마우스 X,Y위치 저장
	float m_XChange, m_YChange; //이번 프레임에서 현재 프레임까지 마우스 이동량 dX, dY 저장
	bool m_MouseFirstMoved; //마우스가 처음 움직이기 시작했는지 여부

	//GLFW 윈도우에 등록할 이벤트 콜백
	//https://www.glfw.org/docs/latest/input_guide.html#input_keyboard
	//콜백을 GLFWWindow에 등록하면, 키보드 및 마우스 입력이 발생할 때마다 함수들이 호출됨
	static void HandleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void HandleMouse(GLFWwindow* window, double xPos, double yPos);
	
	void CreateCallback(); //콜백 등록
};