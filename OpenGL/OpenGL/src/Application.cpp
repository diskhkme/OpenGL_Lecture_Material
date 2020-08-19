#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <crtdbg.h>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "Camera.h"
#include "Model.h"
#include "DirectionalLight.h"
#include "Material.h"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

void ChangeProgramAndMaterial(int& materialNum, bool * keys);

int main(void)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); //memory leak check

	//Window 관련 구현 Window 클래스로 이동
	Window mainWindow{ 800,600 };
	mainWindow.Initialize();
	
	{ 
		//외부 모델을 불러와서 사용하기 위해 Model 클래스 정의
		Model teapot;
		teapot.LoadModel("res/models/teapot.obj"); //obj와 같은 3D 모델 파일을 입력 가능
		
		
		//yaw 값이 0일때는 front가 [1,0,0]이므로, yaw를 90으로 해서 초기 front가 [0,0,-1]이 되도록 함
		Camera camera{ glm::vec3{0.0f,0.0f,15.0f}, glm::vec3{0.0f,1.0f,0.0f}, -90.0f, 0.0f, 5.0f, 0.5f };

		//앞뒤 이동 효과를 올바로 보기 위해서는 perspective projection 행렬 필요
		float aspect = (float)mainWindow.GetBufferWidth() / mainWindow.GetBufferHeight();
		glm::mat4 proj = glm::perspective(45.0f, aspect, 0.1f, 100.0f);

		//--------------Texture 생성---------//
		Texture texture{ "res/textures/uvchecker.jpg" };
		texture.Bind(); //0번 슬롯에 바인딩

		//---------Shader 생성---------------//
		Shader shaderPerFragment{ "res/shaders/Lighting_Specular_Per_Fragment.shader" };
		shaderPerFragment.Bind();
		shaderPerFragment.SetUniformMat4f("u_Model", glm::mat4{ 1.0f }); //Mat4{1.0}은 단위 행렬
		shaderPerFragment.SetUniformMat4f("u_Projection", proj);
		shaderPerFragment.SetUniformMat4f("u_View", camera.CalculateViewMatrix());
		shaderPerFragment.SetUniform1i("u_Texture", 0);
		
		Renderer renderer;

		DirectionalLight mainLight{ glm::vec3{1.0f,1.0f,1.0f}, 0.3f ,
						glm::vec3{2.0f,-1.0f,-2.0f}, 0.3f }; //specular 효과를 잘 보기위해 diffuse를 약간 줄임

		std::vector<Material> materials;
		materials.emplace_back(5.0f, 32.0f); //반사도가 높은 물체. Intensity와 shininess factor가 큼
		materials.emplace_back(0.3f, 4.0f);  //반사도가 낮은 물체
		
		//매 프레임마다 소요되는 시간을 계산/저장 하기 위한 변수
		float deltaTime = 0.0f;
		float lastTime = 0.0f;
		
		int materialNum = 0;

		while (!mainWindow.GetShouldClose())
		{
			float now = glfwGetTime(); //현재 시간
			deltaTime = now - lastTime; //소요 시간 = 현재 시간 - 이전 프레임 시간 
			lastTime = now;

			//poll event 부분은 유저 입력에 필요하므로 남겨둠
			glfwPollEvents();

			camera.KeyControl(mainWindow.GetKeys(), deltaTime);
			camera.MouseControl(mainWindow.GetXChange(), mainWindow.GetYChange());
		
			glm::vec3 camPosition = camera.GetEyePosition();
			
			ChangeProgramAndMaterial(materialNum, mainWindow.GetKeys());

			renderer.Clear();
			
			//per-fragment object
			shaderPerFragment.Bind();
			mainLight.UseLight(shaderPerFragment); //light 관련한 uniform setting
			materials[materialNum].UseMaterial(shaderPerFragment); //반사도 높은 물체
			shaderPerFragment.SetUniformMat4f("u_Model", glm::translate(glm::vec3{ 0.0f, 0.0f, 0.0f })); //Mat4{1.0}은 단위 행렬
			shaderPerFragment.SetUniformMat4f("u_View", camera.CalculateViewMatrix()); //카메라 변화에 따라 새로 계산된 view 행렬 셰이더에 전달
			shaderPerFragment.SetUniform3f("u_EyePosition", camPosition.x, camPosition.y, camPosition.z);
			teapot.RenderModel(shaderPerFragment);
			shaderPerFragment.Unbind();


			/* Swap front and back buffers */
			mainWindow.SwapBuffers();

		}
	}

	
	return 0;
}

void ChangeProgramAndMaterial(int& materialNum, bool * keys)
{
	if (keys[GLFW_KEY_Z])
	{
		materialNum = 0;
	}
	if (keys[GLFW_KEY_X])
	{
		materialNum = 1;
	}

}