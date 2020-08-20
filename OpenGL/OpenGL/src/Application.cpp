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

void ChangeProgramAndMaterial(int& materialNum, int& shaderNum, bool * keys);

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

		Model plane;
		plane.LoadModel("res/models/SubdividedPlane_100.obj");
		
		
		//yaw ���� 0�϶��� front�� [1,0,0]�̹Ƿ�, yaw�� 90���� �ؼ� �ʱ� front�� [0,0,-1]�� �ǵ��� ��
		Camera camera{ glm::vec3{0.0f,10.0f,15.0f}, glm::vec3{0.0f,1.0f,0.0f}, -90.0f, 0.0f, 5.0f, 0.5f };

		//�յ� �̵� ȿ���� �ùٷ� ���� ���ؼ��� perspective projection ��� �ʿ�
		float aspect = (float)mainWindow.GetBufferWidth() / mainWindow.GetBufferHeight();
		glm::mat4 proj = glm::perspective(45.0f, aspect, 0.1f, 100.0f);

		//--------------Texture ����---------//
		Texture texture{ "res/textures/diffuse.png" }; //diffuse map
		
		Texture normalMap{ "res/textures/normal.png" }; //normal map

		//---------Shader ����---------------//
		
		//normal map�� �������� ���� shader
		Shader shaderPerFragment{ "res/shaders/Lighting_Specular_Per_Fragment.shader" };
		shaderPerFragment.Bind();
		shaderPerFragment.SetUniformMat4f("u_Model", glm::mat4{ 1.0f }); //Mat4{1.0}�� ���� ���
		shaderPerFragment.SetUniformMat4f("u_Projection", proj);
		shaderPerFragment.SetUniformMat4f("u_View", camera.CalculateViewMatrix());

		//normal map�� ������ shader
		Shader shaderNormalMap{ "res/shaders/Lighting_And_NormalMap.shader" };
		shaderNormalMap.Bind();
		shaderNormalMap.SetUniformMat4f("u_Model", glm::mat4{ 1.0f }); //Mat4{1.0}�� ���� ���
		shaderNormalMap.SetUniformMat4f("u_Projection", proj);
		shaderNormalMap.SetUniformMat4f("u_View", camera.CalculateViewMatrix());

		//normal map using tangent space transform
		Shader shaderNormalMapTangent{ "res/shaders/Lighting_And_NormalMap_inTangent.shader" };
		shaderNormalMapTangent.Bind();
		shaderNormalMapTangent.SetUniformMat4f("u_Model", glm::mat4{ 1.0f }); //Mat4{1.0}�� ���� ���
		shaderNormalMapTangent.SetUniformMat4f("u_Projection", proj);
		shaderNormalMapTangent.SetUniformMat4f("u_View", camera.CalculateViewMatrix());

		
		Renderer renderer;

		DirectionalLight mainLight{ glm::vec3{1.0f,1.0f,1.0f}, 0.0f ,
						glm::vec3{0.0f,0.0f,-1.0f}, 1.0f }; //specular ȿ���� �� �������� diffuse�� �ణ ����

		std::vector<Material> materials;
		materials.emplace_back(1.0f, 32.0f); //�ݻ絵�� ���� ��ü. Intensity�� shininess factor�� ŭ
		materials.emplace_back(0.3f, 4.0f);  //�ݻ絵�� ���� ��ü
		
		//�� �����Ӹ��� �ҿ�Ǵ� �ð��� ���/���� �ϱ� ���� ����
		float deltaTime = 0.0f;
		float lastTime = 0.0f;
		
		int materialNum = 0;
		int shaderNum = 0;

		float rotAngle = 90.0f;
		float angleIncrement = 5.0f;

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
			
			ChangeProgramAndMaterial(materialNum, shaderNum, mainWindow.GetKeys());

			renderer.Clear();
			
			
			if (rotAngle > 110.0f)
				angleIncrement = -5.0f;

			if (rotAngle < 70.0f)
				angleIncrement = 5.0f;
			
			rotAngle += angleIncrement * deltaTime;

			//shader program�� ���ڷ� �޴� render �Լ��� ������ ���� ��
			if (shaderNum == 0)
			{
				shaderPerFragment.Bind();
				mainLight.UseLight(shaderPerFragment); //light ������ uniform setting
				materials[materialNum].UseMaterial(shaderPerFragment); //�ݻ絵 ���� ��ü
				glm::mat4 planeModelMat = glm::mat4{ 1.0 };
				planeModelMat = glm::rotate(planeModelMat, glm::radians( rotAngle ), glm::vec3{ 1.0f,0.0f,0.0f }); //�ð��� ���� ȸ�� �ִϸ��̼�
				planeModelMat = glm::scale(planeModelMat, glm::vec3{ 0.1f,0.1f,0.1f });
				
				shaderPerFragment.SetUniformMat4f("u_Model", planeModelMat);
				shaderPerFragment.SetUniformMat4f("u_View", camera.CalculateViewMatrix()); //ī�޶� ��ȭ�� ���� ���� ���� view ��� ���̴��� ����
				shaderPerFragment.SetUniform3f("u_EyePosition", camPosition.x, camPosition.y, camPosition.z);
				texture.Bind();
				shaderPerFragment.SetUniform1i("u_Texture", 0);
				normalMap.Bind(1);
				shaderPerFragment.SetUniform1i("u_Normal", 1);
				plane.RenderModel(shaderPerFragment);
				shaderPerFragment.Unbind();
			}
			else if (shaderNum == 1) 
			{
				shaderNormalMap.Bind();
				mainLight.UseLight(shaderNormalMap); //light ������ uniform setting
				materials[materialNum].UseMaterial(shaderNormalMap); //�ݻ絵 ���� ��ü
				glm::mat4 planeModelMat = glm::mat4{ 1.0 };
				planeModelMat = glm::rotate(planeModelMat, glm::radians(rotAngle), glm::vec3{ 1.0f,0.0f,0.0f });
				planeModelMat = glm::scale(planeModelMat, glm::vec3{ 0.1f,0.1f,0.1f });
								
				shaderNormalMap.SetUniformMat4f("u_Model", planeModelMat);
				shaderNormalMap.SetUniformMat4f("u_View", camera.CalculateViewMatrix()); //ī�޶� ��ȭ�� ���� ���� ���� view ��� ���̴��� ����
				shaderNormalMap.SetUniform3f("u_EyePosition", camPosition.x, camPosition.y, camPosition.z);
				texture.Bind();
				shaderNormalMap.SetUniform1i("u_Texture", 0);
				normalMap.Bind(1);
				shaderNormalMap.SetUniform1i("u_Normal", 1);
				plane.RenderModel(shaderNormalMap);
				shaderNormalMap.Unbind();
			}
			else if (shaderNum == 2)
			{
				shaderNormalMapTangent.Bind();
				mainLight.UseLight(shaderNormalMapTangent); //light ������ uniform setting
				materials[materialNum].UseMaterial(shaderNormalMapTangent); //�ݻ絵 ���� ��ü
				glm::mat4 planeModelMat = glm::mat4{ 1.0 };
				planeModelMat = glm::rotate(planeModelMat, glm::radians(rotAngle), glm::vec3{ 1.0f,0.0f,0.0f }); //���� �� ȸ���� ���� ������ ������ �ʾƾ� �ϴµ� ���δ�. �� �׷���?
				planeModelMat = glm::scale(planeModelMat, glm::vec3{ 0.1f,0.1f,0.1f });

				shaderNormalMapTangent.SetUniformMat4f("u_Model", planeModelMat);
				shaderNormalMapTangent.SetUniformMat4f("u_View", camera.CalculateViewMatrix()); //ī�޶� ��ȭ�� ���� ���� ���� view ��� ���̴��� ����
				shaderNormalMapTangent.SetUniform3f("u_EyePosition", camPosition.x, camPosition.y, camPosition.z);
				texture.Bind();
				shaderNormalMapTangent.SetUniform1i("u_Texture", 0);
				normalMap.Bind(1);
				shaderNormalMapTangent.SetUniform1i("u_Normal", 1);
				plane.RenderModel(shaderNormalMapTangent);
				shaderNormalMapTangent.Unbind();
			}
			

			/* Swap front and back buffers */
			mainWindow.SwapBuffers();

		}
	}

	
	return 0;
}

void ChangeProgramAndMaterial(int& materialNum, int& shaderNum, bool * keys)
{
	if (keys[GLFW_KEY_Z])
	{
		materialNum = 0;
	}
	if (keys[GLFW_KEY_X])
	{
		materialNum = 1;
	}

	if (keys[GLFW_KEY_1])
	{
		shaderNum = 0; //normal map ������� �ʴ� shader
	}
	if (keys[GLFW_KEY_2])
	{
		shaderNum = 1; //normal map ����ϴ� shader
	}
	if (keys[GLFW_KEY_3])
	{
		shaderNum = 2; //normal map ����ϴ� shader
	}
}