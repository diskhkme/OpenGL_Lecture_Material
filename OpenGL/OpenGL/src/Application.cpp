#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "Camera.h"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

int main(void)
{
	//Window 관련 구현 Window 클래스로 이동
	Window mainWindow{ 800,600 };
	mainWindow.Initialize();
	
	{ 
		
		float positions[] = { //Vertex마다 텍스처 좌표(uv) 데이터 추가
			-0.5f, -0.5f, 0.0f, 0.0f, //0 
			 0.5f, -0.5f, 1.0f, 0.0f, //1
			 0.5f,  0.5f, 1.0f, 1.0f, //2
			-0.5f,  0.5f, 0.0f, 1.0f  //3
		};

		unsigned int indices[] = { //index buffer를 함께 사용(index는 unsigned 타입임에 유의)
			0, 1, 2, //vertex 012로 이루어진 삼각형
			2, 3, 0  //vertex 230로 이루어진 삼각형
		};

		//vao 생성 VertexArray가 담당
		VertexArray va; 
		VertexBuffer vb{ positions, 4 * 2 * 2 * sizeof(float) }; 
		VertexBufferLayout layout;
		layout.Push<float>(2); //vertex당 2개의 위치를 표현하는 float 데이터
		layout.Push<float>(2); //vertex당 2개의 텍스처 좌표를 표현하는 float 데이터
		va.AddBuffer(vb, layout);

		IndexBuffer ib{ indices, 6 };

		//yaw 값이 0일때는 front가 [1,0,0]이므로, yaw를 90으로 해서 초기 front가 [0,0,-1]이 되도록 함
		Camera camera{ glm::vec3{0.0f,0.0f,5.0f}, glm::vec3{0.0f,1.0f,0.0f}, -90.0f, 0.0f, 5.0f, 0.5f };

		//앞뒤 이동 효과를 올바로 보기 위해서는 perspective projection 행렬 필요
		float aspect = (float)mainWindow.GetBufferWidth() / mainWindow.GetBufferHeight();
		glm::mat4 proj = glm::perspective(45.0f, aspect, 0.1f, 100.0f);

		//---------Shader 생성---------------//
		Shader shader{ "res/shaders/Basic.shader" };
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
		shader.SetUniformMat4f("u_Projection", proj);
		shader.SetUniformMat4f("u_View", camera.calculateViewMatrix());

		//--------------Texture 생성---------//
		Texture texture{ "res/textures/JBNU.png" };
		texture.Bind(); //0번 슬롯에 바인딩
		shader.SetUniform1i("u_Texture", 0); //0번 슬롯의 텍스처를 사용할 것이라는 것을 셰이더에 명시
		
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();
			   
		Renderer renderer;

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
			shader.SetUniformMat4f("u_View", camera.calculateViewMatrix()); //카메라 변화에 따라 새로 계산된 view 행렬 셰이더에 전달

			//Renderer로 이동
			renderer.Draw(va, ib, shader);

			/* Swap front and back buffers */
			mainWindow.SwapBuffers();

		}
	}

	
	return 0;
}