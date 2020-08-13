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

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "imgui/imgui.h" //먼저 include 필요
#include "imgui/imgui_impl_glfw_gl3.h"


//행렬 계산을 위한 GLM 라이브러리 추가
//https://github.com/g-truc/glm
//detail 폴더에 있는 dummy.cpp 프로젝트에서 제외

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Opengl 3.3 버전 사용
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1); //1이면 vsync rate와 같은 속도로 화면 갱신

	// glfwMakeContextCurrent가 호출된 후에 glewInit이 수행되어야 함
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error\n";
	}

	std::cout << glGetString(GL_VERSION) << std::endl; //내 플랫폼의 GL_Version 출력해보기

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

		//알파 채널 처리 방법 (chapter 10에서 다룰 예정)
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

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
		glm::mat4 view = glm::lookAt(glm::vec3{ 0.0f,0.0f,0.5f },
									glm::vec3{ 0.0f,0.0f,0.0f },
									glm::vec3{ 0.0f,1.0f,0.0f });
		glm::mat4 model = glm::translate(glm::vec3{ 0,0,0 });
		glm::mat4 mvp = proj * view*model;

		//---------Shader 생성---------------//
		Shader shader{ "res/shaders/Basic.shader" };
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
		shader.SetUniformMat4f("u_MVP", mvp);

		//--------------Texture 생성---------//
		Texture texture{ "res/textures/JBNU.png" };
		texture.Bind(); //0번 슬롯에 바인딩
		shader.SetUniform1i("u_Texture", 0); //0번 슬롯의 텍스처를 사용할 것이라는 것을 셰이더에 명시
		
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();
			   
		Renderer renderer;

		//----ImGUI Init---//
		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();

		float r = 0.0f;
		float increment = 0.05f;
		glm::vec3 translation{ 0.0f,0.0f,0.0f };
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();

			//imGUI 새 프레임
			ImGui_ImplGlfwGL3_NewFrame();

			glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
			glm::mat4 view = glm::lookAt(glm::vec3{ 0.0f,0.0f,0.5f },
				glm::vec3{ 0.0f,0.0f,0.0f },
				glm::vec3{ 0.0f,1.0f,0.0f });
			glm::mat4 model = glm::translate(translation); //imGUI에서 변경된 translation update
			glm::mat4 mvp = proj * view*model;

			shader.Bind();
			shader.SetUniformMat4f("u_MVP", mvp);
			shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f); //여기서 다시 바인딩하고 데이터를 제공하는 것은 문제는 없음. Material(Shader + data)를 정의해서 분리하는 것이 일반적

			//Renderer로 이동
			renderer.Draw(va, ib, shader);
			
			if (r > 1.0f)
				increment = -0.05f;
			if (r < 0.0f)
				increment = 0.05f;

			r += increment;

			// 1. Show a simple window.
			// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
			{
				static float f = 0.0f;
				ImGui::SliderFloat3("Translation", &translation.x, -1.0f, 1.0f);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}


			//imGUI 렌더
			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}
	
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}