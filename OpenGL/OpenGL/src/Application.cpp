#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"


//Shader Ŭ������ �̵�

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
		
		float positions[] = { //�簢���� �׸��� ���� 2�� ����
			-0.5f, -0.5f, //0
			 0.5f, -0.5f, //1
			 0.5f,  0.5f, //2
			-0.5f,  0.5f, //3
		};

		unsigned int indices[] = { //index buffer�� �Բ� ���(index�� unsigned Ÿ���ӿ� ����)
			0, 1, 2, //vertex 012�� �̷���� �ﰢ��
			2, 3, 0  //vertex 230�� �̷���� �ﰢ��
		};

		//vao ���� VertexArray�� ���
		VertexArray va; 
		VertexBuffer vb{ positions, 4 * 2 * sizeof(float) }; 
		VertexBufferLayout layout;
		layout.Push<float>(2); //vertex�� 2���� ��ġ�� ǥ���ϴ� float ������
		va.AddBuffer(vb, layout);

		IndexBuffer ib{ indices, 6 };


		//---------Shader ����---------------//
		Shader shader{ "res/shaders/Basic.shader" };
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
		
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		float r = 0.0f;
		float increment = 0.05f;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			glClear(GL_COLOR_BUFFER_BIT);

			shader.Bind();
			shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

			va.Bind();
			ib.Bind();

			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); //Draw call

			if (r > 1.0f)
				increment = -0.05f;
			if (r < 0.0f)
				increment = 0.05f;

			r += increment;

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}
	

	glfwTerminate();
	return 0;
}