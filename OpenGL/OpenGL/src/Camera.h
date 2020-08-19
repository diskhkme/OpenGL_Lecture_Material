#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//WASD�� �����̰�, ���콺�� �ѷ����� ī�޶��� ����

class Camera
{
public:
	Camera();
	Camera(glm::vec3 initEye, glm::vec3 initUp,
		float initYaw, float initPitch,
		float initMoveSpeed, float initTurnSpeed);
	~Camera();

	void KeyControl(bool *keys, float deltaTime);
	void MouseControl(float xChange, float yChange);

	glm::mat4 CalculateViewMatrix();
	glm::vec3 GetEyePosition() {return m_Eye;} //specular ����� ���� ī�޶� ��ġ ��ȯ
private:
	glm::vec3 m_Eye, m_Front, m_Up, m_Right, m_WorldUp;
	float m_Yaw, m_Pitch;
	float m_MoveSpeed, m_TurnSpeed;

	void Update();
};