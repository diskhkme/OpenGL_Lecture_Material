#include "TestTexture2D.h"

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

namespace test
{
	TestTexture2D::TestTexture2D()
		: m_TranslationA{0,0,0}, m_TranslationB{0.5f,0,0},
		m_Proj{}, m_View{}
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
		
		m_VAO = std::make_unique<VertexArray>();
		m_VBO = std::make_unique<VertexBuffer>(positions, 4 * 2 * 2 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2); //vertex당 2개의 위치를 표현하는 float 데이터
		layout.Push<float>(2); //vertex당 2개의 텍스처 좌표를 표현하는 float 데이터
		m_VAO->AddBuffer(*m_VBO, layout);

		m_IBO = std::make_unique<IndexBuffer>(indices, 6);

		// GLM orthographics projection matrix 테스트
		m_Proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
		m_View = glm::lookAt(glm::vec3{ 0.0f,0.0f,0.5f },
			glm::vec3{ 0.0f,0.0f,0.0f },
			glm::vec3{ 0.0f,1.0f,0.0f });
		glm::mat4 model = glm::translate(glm::vec3{ 0,0,0 });
		glm::mat4 mvp = m_Proj * m_View*model;

		//---------Shader 생성---------------//
		m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
		m_Shader->Bind();
		m_Shader->SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
		m_Shader->SetUniformMat4f("u_MVP", mvp);

		//--------------Texture 생성---------//
		m_Texture = std::make_unique<Texture>("res/textures/JBNU.png");
		m_Texture->Bind(); //0번 슬롯에 바인딩
		m_Shader->SetUniform1i("u_Texture", 0); //0번 슬롯의 텍스처를 사용할 것이라는 것을 셰이더에 명시
	}

	TestTexture2D::~TestTexture2D()
	{
		//스마트 포인터 활용으로 자동 free
	}

	void TestTexture2D::OnUpdate(float deltaTime)
	{
	}

	void TestTexture2D::OnRender()
	{
		GLCall(glClearColor(0.0f,0.0f,0.0f,1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		Renderer renderer;

		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f),m_TranslationA);

			glm::mat4 mvp = m_Proj * m_View*model;
			//Draw 1st object
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_MVP", mvp);
			renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
		}

		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);

			glm::mat4 mvp = m_Proj * m_View*model;
			//Draw 2nd object
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_MVP", mvp);
			renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
		}
	}

	void TestTexture2D::OnImGuiRender()
	{
		static float f = 0.0f;
		ImGui::SliderFloat3("Translation A", &m_TranslationA.x, -1.0f, 1.0f);
		ImGui::SliderFloat3("Translation B", &m_TranslationB.x, -1.0f, 1.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}

