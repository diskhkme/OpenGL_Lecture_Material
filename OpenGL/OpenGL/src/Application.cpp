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

		//yaw ���� 0�϶��� front�� [1,0,0]�̹Ƿ�, yaw�� 90���� �ؼ� �ʱ� front�� [0,0,-1]�� �ǵ��� ��
		Camera camera{ glm::vec3{0.0f,0.0f,5.0f}, glm::vec3{0.0f,1.0f,0.0f}, -90.0f, 0.0f, 5.0f, 0.5f };

		//�յ� �̵� ȿ���� �ùٷ� ���� ���ؼ��� perspective projection ��� �ʿ�
		float aspect = (float)mainWindow.GetBufferWidth() / mainWindow.GetBufferHeight();
		glm::mat4 proj = glm::perspective(45.0f, aspect, 0.1f, 100.0f);

		//---------Shader ����---------------//
		Shader shader{ "res/shaders/Basic.shader" };
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
		shader.SetUniformMat4f("u_Projection", proj);
		shader.SetUniformMat4f("u_View", camera.calculateViewMatrix());

		//--------------Texture ����---------//
		Texture texture{ "res/textures/JBNU.png" };
		texture.Bind(); //0�� ���Կ� ���ε�
		shader.SetUniform1i("u_Texture", 0); //0�� ������ �ؽ�ó�� ����� ���̶�� ���� ���̴��� ���
		
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();
			   
		Renderer renderer;

		//�� �����Ӹ��� �ҿ�Ǵ� �ð��� ���/���� �ϱ� ���� ����
		float deltaTime = 0.0f;
		float lastTime = 0.0f;
		
		while (!mainWindow.GetShouldClose())
		{
			float now = glfwGetTime(); //���� �ð�
			deltaTime = now - lastTime; //�ҿ� �ð� = ���� �ð� - ���� ������ �ð� 
			lastTime = now;


			//poll event �κ��� ���� �Է¿� �ʿ��ϹǷ� ���ܵ�
			glfwPollEvents();

			camera.KeyControl(mainWindow.GetKeys(), deltaTime);
			camera.MouseControl(mainWindow.GetXChange(), mainWindow.GetYChange());
		
			renderer.Clear();

			shader.Bind();
			shader.SetUniformMat4f("u_View", camera.calculateViewMatrix()); //ī�޶� ��ȭ�� ���� ���� ���� view ��� ���̴��� ����

			//Renderer�� �̵�
			renderer.Draw(va, ib, shader);

			/* Swap front and back buffers */
			mainWindow.SwapBuffers();

		}
	}

	
	return 0;
}