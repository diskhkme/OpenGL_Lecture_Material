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

	//Window ���� ���� Window Ŭ������ �̵�
	Window mainWindow{ 800,600 };
	mainWindow.Initialize();
	
	{ 
		//�ܺ� ���� �ҷ��ͼ� ����ϱ� ���� Model Ŭ���� ����
		Model teapot;
		teapot.LoadModel("res/models/teapot.obj"); //obj�� ���� 3D �� ������ �Է� ����
		
		
		//yaw ���� 0�϶��� front�� [1,0,0]�̹Ƿ�, yaw�� 90���� �ؼ� �ʱ� front�� [0,0,-1]�� �ǵ��� ��
		Camera camera{ glm::vec3{0.0f,0.0f,15.0f}, glm::vec3{0.0f,1.0f,0.0f}, -90.0f, 0.0f, 5.0f, 0.5f };

		//�յ� �̵� ȿ���� �ùٷ� ���� ���ؼ��� perspective projection ��� �ʿ�
		float aspect = (float)mainWindow.GetBufferWidth() / mainWindow.GetBufferHeight();
		glm::mat4 proj = glm::perspective(45.0f, aspect, 0.1f, 100.0f);

		//---------Shader ����---------------//
		Shader shader{ "res/shaders/Basic.shader" };
		shader.Bind();
		shader.SetUniformMat4f("u_Model", glm::mat4{ 1.0f }); //Mat4{1.0}�� ���� ���
		shader.SetUniformMat4f("u_Projection", proj);
		shader.SetUniformMat4f("u_View", camera.calculateViewMatrix());

		//--------------Texture ����---------//
		Texture texture{ "res/textures/uvchecker.jpg" };
		texture.Bind(); //0�� ���Կ� ���ε�
		shader.SetUniform1i("u_Texture", 0); //0�� ������ �ؽ�ó�� ����� ���̶�� ���� ���̴��� ���
		
		Renderer renderer;

		Light mainLight{ glm::vec3{1.0f,1.0f,1.0f}, 0.2f };

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
			mainLight.UseLight(shader); //light ������ uniform setting
			shader.SetUniformMat4f("u_View", camera.calculateViewMatrix()); //ī�޶� ��ȭ�� ���� ���� ���� view ��� ���̴��� ����

			teapot.RenderModel(shader);

			/* Swap front and back buffers */
			mainWindow.SwapBuffers();

		}
	}

	
	return 0;
}