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

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"


//행렬 계산을 위한 GLM 라이브러리 추가
//https://github.com/g-truc/glm
//detail 폴더에 있는 dummy.cpp 프로젝트에서 제외

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

		// GLM orthographics projection matrix 테스트
		glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);

		//---------Shader 생성---------------//
		Shader shader{ "res/shaders/Basic.shader" };
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
		shader.SetUniformMat4f("u_Projection", proj);

		//--------------Texture 생성---------//
		Texture texture{ "res/textures/JBNU.png" };
		texture.Bind(); //0번 슬롯에 바인딩
		shader.SetUniform1i("u_Texture", 0); //0번 슬롯의 텍스처를 사용할 것이라는 것을 셰이더에 명시
		
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();
			   
		Renderer renderer;

		float r = 0.0f;
		float increment = 0.05f;
		/* Loop until the user closes the window */
		while (!mainWindow.GetShouldClose())
		{
			/* Render here */
			renderer.Clear();

			shader.Bind();
			shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f); //여기서 다시 바인딩하고 데이터를 제공하는 것은 문제는 없음. Material(Shader + data)를 정의해서 분리하는 것이 일반적

			//Renderer로 이동
			renderer.Draw(va, ib, shader);
			
			if (r > 1.0f)
				increment = -0.05f;
			if (r < 0.0f)
				increment = 0.05f;

			r += increment;

			/* Swap front and back buffers */
			mainWindow.SwapBuffers();

			//poll event 부분은 유저 입력에 필요하므로 남겨둠
			glfwPollEvents();
		}
	}

	
	return 0;
}