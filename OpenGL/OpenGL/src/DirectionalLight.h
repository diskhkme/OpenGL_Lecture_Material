#pragma once

#include "Light.h"

//Directional Light 유도 클래스
class DirectionalLight : public Light
{
public:
	DirectionalLight();
	//이제 directional light는 shadow map을 그리는 역할까지 포함
	DirectionalLight( int shadowWidth, int shadowHeight,
					glm::vec3 aColor, float aIntensity,
					glm::vec3 direction, float dIntensity);
	~DirectionalLight();

	void UseLight(Shader& shader, int slot); //그림자를 그리기 위한 uniform 설정
	void UseLightForShadow(Shader& shader); //depth map 생성을 위한 uniform 설정

	void SetDirection(const glm::vec3& dir);

	//light 변환 행렬 반환 함수
	glm::mat4 CalculateLightTransform();
private:
	glm::vec3 m_Direction;
};