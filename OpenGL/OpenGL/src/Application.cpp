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
	//Window 관련 구현 Window 클래스로 이동
	Window mainWindow{ bufferWidth,bufferHeight };
	mainWindow.Initialize();
	
	{ 
		//외부 모델을 불러와서 사용하기 위해 Model 클래스 정의
		Model teapot;
		teapot.LoadModel("res/models/teapot.obj"); //obj와 같은 3D 모델 파일을 입력 가능

		Model plane;
		plane.LoadModel("res/models/SubdividedPlane_100.obj");
		
		
		//yaw 값이 0일때는 front가 [1,0,0]이므로, yaw를 90으로 해서 초기 front가 [0,0,-1]이 되도록 함
		Camera camera{ glm::vec3{0.0f,0.0f,15.0f}, glm::vec3{0.0f,1.0f,0.0f}, -90.0f, 0.0f, 5.0f, 0.5f };

		float aspect = (float)mainWindow.GetBufferWidth() / mainWindow.GetBufferHeight();
		glm::mat4 proj = glm::perspective(45.0f, aspect, 0.1f, 100.0f);

		////--------------Texture 생성---------//
		Texture texture{ "res/textures/uvchecker.jpg" };
		//texture.Bind(0); //0번 슬롯에 바인딩
		//Texture toonLutTexture{ "res/textures/TwoToneGradient.tga" };
		Texture toonLutTexture{ "res/textures/UtilToonGradient.tga" };

		//---------Shader 생성---------------//
		Shader shaderPerFragment{ "res/shaders/ToonShading.shader" };
		shaderPerFragment.Bind();
		shaderPerFragment.SetUniformMat4f("u_Model", glm::mat4{ 1.0f }); //Mat4{1.0}은 단위 행렬
		shaderPerFragment.SetUniformMat4f("u_Projection", proj);
		shaderPerFragment.SetUniformMat4f("u_View", camera.CalculateViewMatrix());

		//테스트를 위해 간단히 depth map을 텍스처로 활용하는 shader 작성
		Shader simpleDepthVisualizeShader{ "res/shaders/DepthMapVisualize.shader" };
		simpleDepthVisualizeShader.Bind();
		simpleDepthVisualizeShader.SetUniformMat4f("u_Model", glm::mat4{ 1.0f }); //Mat4{1.0}은 단위 행렬
		simpleDepthVisualizeShader.SetUniformMat4f("u_Projection", proj);
		simpleDepthVisualizeShader.SetUniformMat4f("u_View", camera.CalculateViewMatrix());
		
		//depth map을 그리는 shader
		Shader shaderShadowMap{ "res/shaders/DirectionalShadowMap.shader" };
		
		Renderer renderer;

		//shadow Map을 그리는 Directional Light
		DirectionalLight mainLight{ 1024,1024, //해상도를 바꾸어 어떤 효과가 있는지 보자
						glm::vec3{1.0f,1.0f,1.0f}, 0.1f ,
						glm::vec3{2.0f,-2.0f,0.0f}, 1.0f }; //빛은 위에서 아래로 똑바로 내리쬐고 있음

		std::vector<Material> materials;
		materials.emplace_back(5.0f, 32.0f); //반사도가 높은 물체. Intensity와 shininess factor가 큼
		materials.emplace_back(0.5f, 4.0f);  //반사도가 낮은 물체
		
		//매 프레임마다 소요되는 시간을 계산/저장 하기 위한 변수
		float deltaTime = 0.0f;
		float lastTime = 0.0f;
		
		int sceneNum = 0;

		while (!mainWindow.GetShouldClose())
		{
			float now = glfwGetTime(); //현재 시간
			deltaTime = now - lastTime; //소요 시간 = 현재 시간 - 이전 프레임 시간 
			lastTime = now;

			//poll event 부분은 유저 입력에 필요하므로 남겨둠
			glfwPollEvents();

			camera.KeyControl(mainWindow.GetKeys(), deltaTime);
			camera.MouseControl(mainWindow.GetXChange(), mainWindow.GetYChange());
		
			ChangeScene(sceneNum, mainWindow.GetKeys());

			glm::vec3 camPosition = camera.GetEyePosition();

			mainLight.SetDirection(glm::vec3(cos(now), -1.0f, glm::sin(now)));
			
			//-----------이제 그리기 pass는 두 개로 나뉘어짐 1. shadow map / 2. scene--------//
			
			//1.Shadow Map Pass
			{
				//뷰포트는 shadow map 버퍼 크기로
				int width = mainLight.GetShadowMap()->GetShadowWidth();
				int height = mainLight.GetShadowMap()->GetShadowWidth();
				mainWindow.ChangeViewPort(width, height);
				mainLight.GetShadowMap()->Bind(); //실제로 하는 것은 프레임 버퍼 바인딩
				renderer.Clear(); //depth만 clear해도 됨
				shaderShadowMap.Bind();
				mainLight.UseLightForShadow(shaderShadowMap);

				//모델 렌더링(모델이 많고 씬이 복잡하면 별도의 렌더링 함수로 분리)
				shaderShadowMap.SetUniformMat4f("u_Model", glm::scale(glm::vec3{ 0.3f, 0.3f, 0.3f })); //Mat4{1.0}은 단위 행렬
				teapot.RenderModel(shaderShadowMap);

				glm::mat4 planeModelMat = glm::mat4{ 1.0f };
				planeModelMat = glm::scale( planeModelMat , glm::vec3{0.5f,0.5f,0.5f} );
				planeModelMat = glm::translate(planeModelMat, glm::vec3{ 0.0f,-10.0f,0.0f });
				shaderShadowMap.SetUniformMat4f("u_Model", planeModelMat); //Mat4{1.0}은 단위 행렬
				plane.RenderModel(shaderShadowMap); //plane을 그림자가 비칠 바닥면으로 설정

				mainLight.GetShadowMap()->Unbind(); //frame buffer를 unbind해야 pass 2에서 화면에 그림
			}


			//2.Render Pass
			{
				if (sceneNum == 0)
				{
					renderer.Clear();
					mainWindow.ChangeViewPort(bufferWidth, bufferHeight);
					shaderPerFragment.Bind();
					//새로 추가한 Shadow 그리기 관련 uniform
					mainLight.GetShadowMap()->Read(1); //119 line의 슬롯과 같아야 함
					mainLight.UseLight(shaderPerFragment, 1); //light 관련한 uniform setting
					materials[0].UseMaterial(shaderPerFragment); //주전자는 반사도 높게
					shaderPerFragment.SetUniformMat4f("u_View", camera.CalculateViewMatrix()); //카메라 변화에 따라 새로 계산된 view 행렬 셰이더에 전달
					shaderPerFragment.SetUniformMat4f("u_Model", glm::scale(glm::vec3{ 0.3f, 0.3f, 0.3f }));
					shaderPerFragment.SetUniform3f("u_EyePosition", camPosition.x, camPosition.y, camPosition.z);
					texture.Bind(0); //0번 슬롯에 바인딩
					shaderPerFragment.SetUniform1i("u_Texture", 0);
					toonLutTexture.Bind(1);
					shaderPerFragment.SetUniform1i("u_ToonLut", 1);
					teapot.RenderModel(shaderPerFragment);
					

					//glm::mat4 planeModelMat = glm::mat4{ 1.0f };
					//planeModelMat = glm::scale(planeModelMat, glm::vec3{ 0.5f,0.5f,0.5f });
					//planeModelMat = glm::translate(planeModelMat, glm::vec3{ 0.0f,-10.0f,0.0f });
					//shaderPerFragment.SetUniformMat4f("u_Model", planeModelMat); //Mat4{1.0}은 단위 행렬
					//materials[1].UseMaterial(shaderPerFragment); //바닥면은 반사도 낮게
					//plane.RenderModel(shaderPerFragment);
					//shaderPerFragment.Unbind();
				}
				if (sceneNum == 1)
				{
					renderer.Clear();
					mainWindow.ChangeViewPort(bufferWidth, bufferHeight);
					simpleDepthVisualizeShader.Bind();
					//새로 추가한 Shadow 그리기 관련 uniform
					mainLight.GetShadowMap()->Read(1); //119 line의 슬롯과 같아야 함
					//mainLight.UseLight(simpleDepthVisualizeShader, 1); //light 관련한 uniform setting
					//materials[materialNum].UseMaterial(shaderPerFragment); //반사도 높은 물체
					simpleDepthVisualizeShader.SetUniformMat4f("u_Model", glm::rotate(glm::scale(glm::vec3{ 0.1f, 0.1f, 0.1f }), 90.0f, glm::vec3{ 1.0f,0.0f,0.0f })); //Mat4{1.0}은 단위 행렬
					simpleDepthVisualizeShader.SetUniformMat4f("u_View", camera.CalculateViewMatrix()); //카메라 변화에 따라 새로 계산된 view 행렬 셰이더에 전달
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