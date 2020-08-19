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

		//--------------Texture ����---------//
		Texture texture{ "res/textures/uvchecker.jpg" };
		texture.Bind(); //0�� ���Կ� ���ε�

		//---------Shader ����---------------//
		Shader shaderPerFragment{ "res/shaders/Lighting_Specular_Per_Fragment.shader" };
		shaderPerFragment.Bind();
		shaderPerFragment.SetUniformMat4f("u_Model", glm::mat4{ 1.0f }); //Mat4{1.0}�� ���� ���
		shaderPerFragment.SetUniformMat4f("u_Projection", proj);
		shaderPerFragment.SetUniformMat4f("u_View", camera.CalculateViewMatrix());
		shaderPerFragment.SetUniform1i("u_Texture", 0);
		
		Renderer renderer;

		DirectionalLight mainLight{ glm::vec3{1.0f,1.0f,1.0f}, 0.3f ,
						glm::vec3{2.0f,-1.0f,-2.0f}, 0.3f }; //specular ȿ���� �� �������� diffuse�� �ణ ����

		std::vector<Material> materials;
		materials.emplace_back(5.0f, 32.0f); //�ݻ絵�� ���� ��ü. Intensity�� shininess factor�� ŭ
		materials.emplace_back(0.3f, 4.0f);  //�ݻ絵�� ���� ��ü
		
		//�� �����Ӹ��� �ҿ�Ǵ� �ð��� ���/���� �ϱ� ���� ����
		float deltaTime = 0.0f;
		float lastTime = 0.0f;
		
		int materialNum = 0;

		while (!mainWindow.GetShouldClose())
		{
			float now = glfwGetTime(); //���� �ð�
			deltaTime = now - lastTime; //�ҿ� �ð� = ���� �ð� - ���� ������ �ð� 
			lastTime = now;

			//poll event �κ��� ���� �Է¿� �ʿ��ϹǷ� ���ܵ�
			glfwPollEvents();

			camera.KeyControl(mainWindow.GetKeys(), deltaTime);
			camera.MouseControl(mainWindow.GetXChange(), mainWindow.GetYChange());
		
			glm::vec3 camPosition = camera.GetEyePosition();
			
			ChangeProgramAndMaterial(materialNum, mainWindow.GetKeys());

			renderer.Clear();
			
			//per-fragment object
			shaderPerFragment.Bind();
			mainLight.UseLight(shaderPerFragment); //light ������ uniform setting
			materials[materialNum].UseMaterial(shaderPerFragment); //�ݻ絵 ���� ��ü
			shaderPerFragment.SetUniformMat4f("u_Model", glm::translate(glm::vec3{ 0.0f, 0.0f, 0.0f })); //Mat4{1.0}�� ���� ���
			shaderPerFragment.SetUniformMat4f("u_View", camera.CalculateViewMatrix()); //ī�޶� ��ȭ�� ���� ���� ���� view ��� ���̴��� ����
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