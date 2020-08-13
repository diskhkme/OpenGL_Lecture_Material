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

#include "imgui/imgui.h" //���� include �ʿ�
#include "imgui/imgui_impl_glfw_gl3.h"


//��� ����� ���� GLM ���̺귯�� �߰�
//https://github.com/g-truc/glm
//detail ������ �ִ� dummy.cpp ������Ʈ���� ����

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Opengl 3.3 ���� ���
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

	glfwSwapInterval(1); //1�̸� vsync rate�� ���� �ӵ��� ȭ�� ����

	// glfwMakeContextCurrent�� ȣ��� �Ŀ� glewInit�� ����Ǿ�� ��
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error\n";
	}

	std::cout << glGetString(GL_VERSION) << std::endl; //�� �÷����� GL_Version ����غ���

	{ 
		
		float positions[] = { //Vertex���� �ؽ�ó ��ǥ(uv) ������ �߰�
			-0.5f, -0.5f, 0.0f, 0.0f, //0 
			 0.5f, -0.5f, 1.0f, 0.0f, //1
			 0.5f,  0.5f, 1.0f, 1.0f, //2
			-0.5f,  0.5f, 0.0f, 1.0f  //3
		};

		unsigned int indices[] = { //index buffer�� �Բ� ���(index�� unsigned Ÿ���ӿ� ����)
			0, 1, 2, //vertex 012�� �̷���� �ﰢ��
			2, 3, 0  //vertex 230�� �̷���� �ﰢ��
		};

		//���� ä�� ó�� ��� (chapter 10���� �ٷ� ����)
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		//vao ���� VertexArray�� ���
		VertexArray va; 
		VertexBuffer vb{ positions, 4 * 2 * 2 * sizeof(float) }; 
		VertexBufferLayout layout;
		layout.Push<float>(2); //vertex�� 2���� ��ġ�� ǥ���ϴ� float ������
		layout.Push<float>(2); //vertex�� 2���� �ؽ�ó ��ǥ�� ǥ���ϴ� float ������
		va.AddBuffer(vb, layout);

		IndexBuffer ib{ indices, 6 };

		// GLM orthographics projection matrix �׽�Ʈ
		glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
		glm::mat4 view = glm::lookAt(glm::vec3{ 0.0f,0.0f,0.5f },
									glm::vec3{ 0.0f,0.0f,0.0f },
									glm::vec3{ 0.0f,1.0f,0.0f });
		glm::mat4 model = glm::translate(glm::vec3{ 0,0,0 });
		glm::mat4 mvp = proj * view*model;

		//---------Shader ����---------------//
		Shader shader{ "res/shaders/Basic.shader" };
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
		shader.SetUniformMat4f("u_MVP", mvp);

		//--------------Texture ����---------//
		Texture texture{ "res/textures/JBNU.png" };
		texture.Bind(); //0�� ���Կ� ���ε�
		shader.SetUniform1i("u_Texture", 0); //0�� ������ �ؽ�ó�� ����� ���̶�� ���� ���̴��� ���
		
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

			//imGUI �� ������
			ImGui_ImplGlfwGL3_NewFrame();

			glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
			glm::mat4 view = glm::lookAt(glm::vec3{ 0.0f,0.0f,0.5f },
				glm::vec3{ 0.0f,0.0f,0.0f },
				glm::vec3{ 0.0f,1.0f,0.0f });
			glm::mat4 model = glm::translate(translation); //imGUI���� ����� translation update
			glm::mat4 mvp = proj * view*model;

			shader.Bind();
			shader.SetUniformMat4f("u_MVP", mvp);
			shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f); //���⼭ �ٽ� ���ε��ϰ� �����͸� �����ϴ� ���� ������ ����. Material(Shader + data)�� �����ؼ� �и��ϴ� ���� �Ϲ���

			//Renderer�� �̵�
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


			//imGUI ����
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