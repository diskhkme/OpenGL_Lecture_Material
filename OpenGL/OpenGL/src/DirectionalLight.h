#pragma once

#include "Light.h"

//Directional Light ���� Ŭ����
class DirectionalLight : public Light
{
public:
	DirectionalLight();
	//���� directional light�� shadow map�� �׸��� ���ұ��� ����
	DirectionalLight( int shadowWidth, int shadowHeight,
					glm::vec3 aColor, float aIntensity,
					glm::vec3 direction, float dIntensity);
	~DirectionalLight();

	void UseLight(Shader& shader, int slot); //�׸��ڸ� �׸��� ���� uniform ����
	void UseLightForShadow(Shader& shader); //depth map ������ ���� uniform ����

	void SetDirection(const glm::vec3& dir);

	//light ��ȯ ��� ��ȯ �Լ�
	glm::mat4 CalculateLightTransform();
private:
	glm::vec3 m_Direction;
};