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


//��� ����� ���� GLM ���̺귯�� �߰�
//https://github.com/g-truc/glm
//detail ������ �ִ� dummy.cpp ������Ʈ���� ����

int main(void)
{
	//Window ���� ���� Window Ŭ������ �̵�
	Window mainWindow{ 800,600 };
	mainWindow.Initialize();
	
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

		//---------Shader ����---------------//
		Shader shader{ "res/shaders/Basic.shader" };
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
		shader.SetUniformMat4f("u_Projection", proj);

		//--------------Texture ����---------//
		Texture texture{ "res/textures/JBNU.png" };
		texture.Bind(); //0�� ���Կ� ���ε�
		shader.SetUniform1i("u_Texture", 0); //0�� ������ �ؽ�ó�� ����� ���̶�� ���� ���̴��� ���
		
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
			shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f); //���⼭ �ٽ� ���ε��ϰ� �����͸� �����ϴ� ���� ������ ����. Material(Shader + data)�� �����ؼ� �и��ϴ� ���� �Ϲ���

			//Renderer�� �̵�
			renderer.Draw(va, ib, shader);
			
			if (r > 1.0f)
				increment = -0.05f;
			if (r < 0.0f)
				increment = 0.05f;

			r += increment;

			/* Swap front and back buffers */
			mainWindow.SwapBuffers();

			//poll event �κ��� ���� �Է¿� �ʿ��ϹǷ� ���ܵ�
			glfwPollEvents();
		}
	}

	
	return 0;
}