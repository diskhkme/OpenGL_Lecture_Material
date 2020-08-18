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
#include "Light.h"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

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

		//---------Shader 생성---------------//
		Shader shader{ "res/shaders/Basic.shader" };
		shader.Bind();
		shader.SetUniformMat4f("u_Model", glm::mat4{ 1.0f }); //Mat4{1.0}은 단위 행렬
		shader.SetUniformMat4f("u_Projection", proj);
		shader.SetUniformMat4f("u_View", camera.calculateViewMatrix());

		//--------------Texture 생성---------//
		Texture texture{ "res/textures/uvchecker.jpg" };
		texture.Bind(); //0번 슬롯에 바인딩
		shader.SetUniform1i("u_Texture", 0); //0번 슬롯의 텍스처를 사용할 것이라는 것을 셰이더에 명시
		
		Renderer renderer;

		Light mainLight{ glm::vec3{1.0f,1.0f,1.0f}, 0.2f };

		//매 프레임마다 소요되는 시간을 계산/저장 하기 위한 변수
		float deltaTime = 0.0f;
		float lastTime = 0.0f;
		
		while (!mainWindow.GetShouldClose())
		{
			float now = glfwGetTime(); //현재 시간
			deltaTime = now - lastTime; //소요 시간 = 현재 시간 - 이전 프레임 시간 
			lastTime = now;

			//poll event 부분은 유저 입력에 필요하므로 남겨둠
			glfwPollEvents();

			camera.KeyControl(mainWindow.GetKeys(), deltaTime);
			camera.MouseControl(mainWindow.GetXChange(), mainWindow.GetYChange());
		
			renderer.Clear();

			shader.Bind();
			mainLight.UseLight(shader); //light 관련한 uniform setting
			shader.SetUniformMat4f("u_View", camera.calculateViewMatrix()); //카메라 변화에 따라 새로 계산된 view 행렬 셰이더에 전달

			teapot.RenderModel(shader);

			/* Swap front and back buffers */
			mainWindow.SwapBuffers();

		}
	}

	
	return 0;
}