#include "Camera.h"

#include <GLFW/glfw3.h>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

Camera::Camera()
{
}

Camera::Camera(glm::vec3 initEye, glm::vec3 initUp, float initYaw, float initPitch, float initMoveSpeed, float initTurnSpeed)
	: m_Eye{ initEye }, m_Front{ glm::vec3{0.0f,0.0f,-1.0f}}, m_WorldUp{initUp},
	m_Pitch{initPitch}, m_Yaw{initYaw},
	m_MoveSpeed{initMoveSpeed}, m_TurnSpeed{initTurnSpeed}
{
	Update();
}

Camera::~Camera()
{
}

void Camera::KeyControl(bool * keys, float deltaTime)
	//keys에는 Window.m_Keys 데이터가 전달됨
{
	//movespeed는 WASD 키를 눌렀을 때 카메라의 이동 속도.
	//deltiTime은 이전 프레임과 현재 프레임 사이의 소요 시간
	float velocity = m_MoveSpeed * deltaTime;

	//WASD가 눌렸을 때는 카메라의 EYE 위치를 이동시키면 됨
	//이때, front는 (at-eye).normalize() 값으로, 마우스 움직임에 따라 eye 좌표를 저장하는 것 보다는
	//at-eye 벡터를 front로 저장하는 편이 편리하므로 이렇게 구현
	//front와 up이 정의되어 있으면, right 방향은 외적으로 손쉽게 구할 수 있음
	if (keys[GLFW_KEY_W])
	{
		m_Eye += m_Front * velocity;
	}
	if (keys[GLFW_KEY_S])
	{
		m_Eye -= m_Front * velocity;
	}
	if (keys[GLFW_KEY_D])
	{
		m_Eye += m_Right * velocity;
	}
	if (keys[GLFW_KEY_A])
	{
		m_Eye -= m_Right * velocity;
	}

}

void Camera::MouseControl(float xChange, float yChange)
	//xChange,yChange에는 Window.m_XChange, Window.m_YChange가 전달됨
{
	//마우스를 1픽셀만큼 움직였을 때 얼마나 각도를 변경할 것인가가 m_TurnSpeed 파라메터
	xChange *= m_TurnSpeed;
	yChange *= m_TurnSpeed;

	m_Yaw += xChange;
	m_Pitch += yChange;

	//y방향으로 많이 움직였을 시 고개를 넘기지 못하도록 각도 제한을 둠
	if (m_Pitch > 89.0f)
		m_Pitch = 89.0f;
	
	if (m_Pitch < -89.0f)
		m_Pitch = -89.0f;

	Update();
}

glm::mat4 Camera::calculateViewMatrix()
{
	glm::mat4 view = glm::lookAt(m_Eye, m_Eye + m_Front, m_Up);
	//std::cout << glm::to_string(view) << std::endl;
	return glm::lookAt(m_Eye, m_Eye + m_Front, m_Up); //At = Eye + Front
}

void Camera::Update()
{
	//마우스 입력에 따른 Front 벡터의 변화 계산 (구면 좌표 -> 카르테시안 좌표)
	m_Front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front.y = sin(glm::radians(m_Pitch));
	m_Front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front = glm::normalize(m_Front);

	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

