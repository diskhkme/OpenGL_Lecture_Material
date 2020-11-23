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

void ChangeScene(int& sceneNum, bool* keys);

int main(void)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); //memory leak check

	int bufferWidth = 800;
	int bufferHeight = 600;
	//Window ���� ���� Window Ŭ������ �̵�
	Window mainWindow{ bufferWidth,bufferHeight };
	mainWindow.Initialize();
	
	{ 
		//�ܺ� ���� �ҷ��ͼ� ����ϱ� ���� Model Ŭ���� ����
		Model teapot;
		teapot.LoadModel("res/models/teapot.obj"); //obj�� ���� 3D �� ������ �Է� ����

		Model plane;
		plane.LoadModel("res/models/SubdividedPlane_100.obj");
		
		
		//yaw ���� 0�϶��� front�� [1,0,0]�̹Ƿ�, yaw�� 90���� �ؼ� �ʱ� front�� [0,0,-1]�� �ǵ��� ��
		Camera camera{ glm::vec3{0.0f,0.0f,15.0f}, glm::vec3{0.0f,1.0f,0.0f}, -90.0f, 0.0f, 5.0f, 0.5f };

		float aspect = (float)mainWindow.GetBufferWidth() / mainWindow.GetBufferHeight();
		glm::mat4 proj = glm::perspective(45.0f, aspect, 0.1f, 100.0f);

		////--------------Texture ����---------//
		Texture texture{ "res/textures/uvchecker.jpg" };
		//texture.Bind(0); //0�� ���Կ� ���ε�
		//Texture toonLutTexture{ "res/textures/TwoToneGradient.tga" };
		Texture toonLutTexture{ "res/textures/UtilToonGradient.tga" };

		//---------Shader ����---------------//
		Shader shaderPerFragment{ "res/shaders/ToonShading.shader" };
		shaderPerFragment.Bind();
		shaderPerFragment.SetUniformMat4f("u_Model", glm::mat4{ 1.0f }); //Mat4{1.0}�� ���� ���
		shaderPerFragment.SetUniformMat4f("u_Projection", proj);
		shaderPerFragment.SetUniformMat4f("u_View", camera.CalculateViewMatrix());

		//�׽�Ʈ�� ���� ������ depth map�� �ؽ�ó�� Ȱ���ϴ� shader �ۼ�
		Shader simpleDepthVisualizeShader{ "res/shaders/DepthMapVisualize.shader" };
		simpleDepthVisualizeShader.Bind();
		simpleDepthVisualizeShader.SetUniformMat4f("u_Model", glm::mat4{ 1.0f }); //Mat4{1.0}�� ���� ���
		simpleDepthVisualizeShader.SetUniformMat4f("u_Projection", proj);
		simpleDepthVisualizeShader.SetUniformMat4f("u_View", camera.CalculateViewMatrix());
		
		//depth map�� �׸��� shader
		Shader shaderShadowMap{ "res/shaders/DirectionalShadowMap.shader" };
		
		Renderer renderer;

		//shadow Map�� �׸��� Directional Light
		DirectionalLight mainLight{ 1024,1024, //�ػ󵵸� �ٲپ� � ȿ���� �ִ��� ����
						glm::vec3{1.0f,1.0f,1.0f}, 0.1f ,
						glm::vec3{2.0f,-2.0f,0.0f}, 1.0f }; //���� ������ �Ʒ��� �ȹٷ� �����ذ� ����

		std::vector<Material> materials;
		materials.emplace_back(5.0f, 32.0f); //�ݻ絵�� ���� ��ü. Intensity�� shininess factor�� ŭ
		materials.emplace_back(0.5f, 4.0f);  //�ݻ絵�� ���� ��ü
		
		//�� �����Ӹ��� �ҿ�Ǵ� �ð��� ���/���� �ϱ� ���� ����
		float deltaTime = 0.0f;
		float lastTime = 0.0f;
		
		int sceneNum = 0;

		while (!mainWindow.GetShouldClose())
		{
			float now = glfwGetTime(); //���� �ð�
			deltaTime = now - lastTime; //�ҿ� �ð� = ���� �ð� - ���� ������ �ð� 
			lastTime = now;

			//poll event �κ��� ���� �Է¿� �ʿ��ϹǷ� ���ܵ�
			glfwPollEvents();

			camera.KeyControl(mainWindow.GetKeys(), deltaTime);
			camera.MouseControl(mainWindow.GetXChange(), mainWindow.GetYChange());
		
			ChangeScene(sceneNum, mainWindow.GetKeys());

			glm::vec3 camPosition = camera.GetEyePosition();

			mainLight.SetDirection(glm::vec3(cos(now), -1.0f, glm::sin(now)));
			
			//-----------���� �׸��� pass�� �� ���� �������� 1. shadow map / 2. scene--------//
			
			//1.Shadow Map Pass
			{
				//����Ʈ�� shadow map ���� ũ���
				int width = mainLight.GetShadowMap()->GetShadowWidth();
				int height = mainLight.GetShadowMap()->GetShadowWidth();
				mainWindow.ChangeViewPort(width, height);
				mainLight.GetShadowMap()->Bind(); //������ �ϴ� ���� ������ ���� ���ε�
				renderer.Clear(); //depth�� clear�ص� ��
				shaderShadowMap.Bind();
				mainLight.UseLightForShadow(shaderShadowMap);

				//�� ������(���� ���� ���� �����ϸ� ������ ������ �Լ��� �и�)
				shaderShadowMap.SetUniformMat4f("u_Model", glm::scale(glm::vec3{ 0.3f, 0.3f, 0.3f })); //Mat4{1.0}�� ���� ���
				teapot.RenderModel(shaderShadowMap);

				glm::mat4 planeModelMat = glm::mat4{ 1.0f };
				planeModelMat = glm::scale( planeModelMat , glm::vec3{0.5f,0.5f,0.5f} );
				planeModelMat = glm::translate(planeModelMat, glm::vec3{ 0.0f,-10.0f,0.0f });
				shaderShadowMap.SetUniformMat4f("u_Model", planeModelMat); //Mat4{1.0}�� ���� ���
				plane.RenderModel(shaderShadowMap); //plane�� �׸��ڰ� ��ĥ �ٴڸ����� ����

				mainLight.GetShadowMap()->Unbind(); //frame buffer�� unbind�ؾ� pass 2���� ȭ�鿡 �׸�
			}


			//2.Render Pass
			{
				if (sceneNum == 0)
				{
					renderer.Clear();
					mainWindow.ChangeViewPort(bufferWidth, bufferHeight);
					shaderPerFragment.Bind();
					//���� �߰��� Shadow �׸��� ���� uniform
					mainLight.GetShadowMap()->Read(1); //119 line�� ���԰� ���ƾ� ��
					mainLight.UseLight(shaderPerFragment, 1); //light ������ uniform setting
					materials[0].UseMaterial(shaderPerFragment); //�����ڴ� �ݻ絵 ����
					shaderPerFragment.SetUniformMat4f("u_View", camera.CalculateViewMatrix()); //ī�޶� ��ȭ�� ���� ���� ���� view ��� ���̴��� ����
					shaderPerFragment.SetUniformMat4f("u_Model", glm::scale(glm::vec3{ 0.3f, 0.3f, 0.3f }));
					shaderPerFragment.SetUniform3f("u_EyePosition", camPosition.x, camPosition.y, camPosition.z);
					texture.Bind(0); //0�� ���Կ� ���ε�
					shaderPerFragment.SetUniform1i("u_Texture", 0);
					toonLutTexture.Bind(1);
					shaderPerFragment.SetUniform1i("u_ToonLut", 1);
					teapot.RenderModel(shaderPerFragment);
					

					//glm::mat4 planeModelMat = glm::mat4{ 1.0f };
					//planeModelMat = glm::scale(planeModelMat, glm::vec3{ 0.5f,0.5f,0.5f });
					//planeModelMat = glm::translate(planeModelMat, glm::vec3{ 0.0f,-10.0f,0.0f });
					//shaderPerFragment.SetUniformMat4f("u_Model", planeModelMat); //Mat4{1.0}�� ���� ���
					//materials[1].UseMaterial(shaderPerFragment); //�ٴڸ��� �ݻ絵 ����
					//plane.RenderModel(shaderPerFragment);
					//shaderPerFragment.Unbind();
				}
				if (sceneNum == 1)
				{
					renderer.Clear();
					mainWindow.ChangeViewPort(bufferWidth, bufferHeight);
					simpleDepthVisualizeShader.Bind();
					//���� �߰��� Shadow �׸��� ���� uniform
					mainLight.GetShadowMap()->Read(1); //119 line�� ���԰� ���ƾ� ��
					//mainLight.UseLight(simpleDepthVisualizeShader, 1); //light ������ uniform setting
					//materials[materialNum].UseMaterial(shaderPerFragment); //�ݻ絵 ���� ��ü
					simpleDepthVisualizeShader.SetUniformMat4f("u_Model", glm::rotate(glm::scale(glm::vec3{ 0.1f, 0.1f, 0.1f }), 90.0f, glm::vec3{ 1.0f,0.0f,0.0f })); //Mat4{1.0}�� ���� ���
					simpleDepthVisualizeShader.SetUniformMat4f("u_View", camera.CalculateViewMatrix()); //ī�޶� ��ȭ�� ���� ���� ���� view ��� ���̴��� ����
					simpleDepthVisualizeShader.SetUniform1i("u_DepthMap", 1);
					//shaderPerFragment.SetUniform3f("u_EyePosition", camPosition.x, camPosition.y, camPosition.z);
					
					
					plane.RenderModel(simpleDepthVisualizeShader);
				}
			}
			
			/* Swap front and back buffers */
			mainWindow.SwapBuffers();

		}
	}

	
	return 0;
}

void ChangeScene(int& sceneNum, bool* keys)
{
	if (keys[GLFW_KEY_1])
	{
		sceneNum = 0;
	}
	if (keys[GLFW_KEY_2])
	{
		sceneNum = 1;
	}
}