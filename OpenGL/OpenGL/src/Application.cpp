#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragSource;
};

//셰이더 파일 파싱 함수
static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};
	
	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos) //vertex 셰이더 섹션
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) //fragment 셰이더 섹션
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n'; //코드를 stringstream에 삽입
		}
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type); //셰이더 객체 생성(마찬가지)
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr); // 셰이더의 소스 코드 명시
	glCompileShader(id); // id에 해당하는 셰이더 컴파일
	
	// Error Handling(없으면 셰이더 프로그래밍할때 괴롭다...)
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result); //셰이더 프로그램으로부터 컴파일 결과(log)를 얻어옴
	if (result == GL_FALSE) //컴파일에 실패한 경우
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); //log의 길이를 얻어옴
		char* message = (char*)alloca(length * sizeof(char)); //stack에 동적할당
		glGetShaderInfoLog(id, length, &length, message); //길이만큼 log를 얻어옴
		std::cout << "셰이더 컴파일 실패! " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id); //컴파일 실패한 경우 셰이더 삭제
		return 0;
	}

	return id;
}

//--------Shader 프로그램 생성, 컴파일, 링크----------//
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragShader)
{
	unsigned int program = glCreateProgram(); //셰이더 프로그램 객체 생성(int에 저장되는 것은 id)
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader); 
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragShader);

	//컴파일된 셰이더 코드를 program에 추가하고 링크
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	//셰이더 프로그램을 생성했으므로 vs, fs 개별 프로그램은 더이상 필요 없음
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// glfwMakeContextCurrent가 호출된 후에 glewInit이 수행되어야 함
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error\n";
	}

	std::cout << glGetString(GL_VERSION) << std::endl; //내 플랫폼의 GL_Version 출력해보기

	//-----Modern OpenGL 방식--------//
	// http://docs.gl/ 에서 아래 gl함수들의 reference를 찾을 수 있음

	//float positions[] = { //사각형을 그리기 위해 1차 수정
	//	-0.5f, -0.5f,
	//	 0.5f, -0.5f,
	//	 0.5f,  0.5f,

	//	 0.5f,  0.5f, //데이터 중복
	//	-0.5f,  0.5f,
	//	-0.5f, -0.5f, //데이터 중복, 현재는 위치 정보만 담고 있지만 일반적으로 법선 등 많은 정보를 담으므로, 낭비가 큼
	//};

	float positions[] = { //사각형을 그리기 위해 2차 수정
		-0.5f, -0.5f, //0
		 0.5f, -0.5f, //1
		 0.5f,  0.5f, //2
		-0.5f,  0.5f, //3
	};

	unsigned int indices[] = { //index buffer를 함께 사용(index는 unsigned 타입임에 유의)
		0, 1, 2, //vertex 012로 이루어진 삼각형
		2, 3, 0  //vertex 230로 이루어진 삼각형
	};

	//---------데이터를 전달하는 과정--------//
	unsigned int bufferID;
	glGenBuffers(1, &bufferID); //1. 버퍼 생성
	glBindBuffer(GL_ARRAY_BUFFER, bufferID); //2. 바인딩("작업 상태")
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positions, GL_STATIC_DRAW);  //3. 작업 상태 버퍼에 데이터 전달

	//---------데이터를 해석하는(법을 정의하는) 과정--------//
	glEnableVertexAttribArray(0); //1. 몇 번째 Location의 attribute를 활성화(enable)
	glVertexAttribPointer(0, 2,	GL_FLOAT, GL_FALSE, sizeof(float)*2, 0); //2. 데이터 해석 방법을 전달.

	unsigned int ibo; //index buffer object
	glGenBuffers(1, &ibo); //1. 인덱스 버퍼 생성
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); //2. 바인딩("작업 상태"), 이번에는 ARRAY_BUFFER가 아니라 ELEMENT_ARRAY_BUFFER
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);  //3. 작업 상태 버퍼에 데이터 전달
	   
	//---------Shader 생성---------------//
	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader"); //셰이더 코드 파싱

	unsigned int shader = CreateShader(source.VertexSource, source.FragSource);
	glUseProgram(shader); //BindBuffer와 마찬가지로, 현재 셰이더 프로그램을 "작업 상태"로 놓음
						  //draw call은 작업 상태인 셰이더 프로그램을 사용하여 작업 상태인 버퍼 데이터를 그림

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		//glDrawArrays(GL_TRIANGLES, 0, 6); //Draw call
		glDrawElements(GL_TRIANGLES, //index buffer를 사용할 때는 drawelement함수를 사용해야 함
						6,			//그릴 index의 갯수
						GL_UNSIGNED_INT, //index 타입 (GL_INT같이 잘못쓰면 안됨!)
						nullptr); //offset 포인터
		

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader); //셰이더 삭제

	glfwTerminate();
	return 0;
}