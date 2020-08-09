#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// 이번에는 Vertex Array와 Shader를 이용하여 삼각형을 그리는 Modern OpenGL 방식으로 구현할 것임
// Vertex Array는 GPU의 VRAM에 Buffer에 저장되는 데이터를 넘기는 방식을 이야기함
// 데이터를 넘기고 나서 삼각형을 실제로 그리는 명령을 호출하는 것을 Draw call이라고 함
// 삼각형이 어떻게 그려질지 사용자가 구현한 프로그램이 Shader임

// OpenGL은 State Machine과 비슷함. 일반적인 객체 지향 프로그램의 설계와는 다르게,
// 현재 처리할 데이터를 선택한 후, 현재 실행해야 할 작업을 함수 호출을 통해 처리하는 방식임
// 간단하게 두 삼각형을 화면에 그리는 의사코드로 설명하면,

// ---객체지향 방식
// Triangle t1, t2; //삼각형 두 개를 정의
// Draw(t1); //t1 객체를 전달함으로써 삼각형 1을 그림
// Draw(t2); //t2 객체를 전달함으로써 삼각형 2를 그림

// ---State Machine 방식
// Triangle t1, t2; //삼각형 두 개를 정의
// Activate(t1); //삼각형 1을 처리중인 상태로 설정
// Draw(); //현재 처리중인 데이터(=삼각형 1)를 화면에 그림
// Activate(t2); //삼각형 2를 처리중인 상태로 설정
// Draw(); //현재 처리중인 데이터(=삼각형 2)를 화면에 그림

//--------Shader 컴파일 함수----------//
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type); //셰이더 객체 생성(마찬가지)
	const char* src = source.c_str();
	glShaderSource(id, // 셰이더의 소스 코드 명시, 소스 코드를 명시할 셰이더 객체 id
					1, // 몇 개의 소스 코드를 명시할 것인지
					&src, // 실제 소스 코드가 들어있는 문자열의 주소값
					nullptr); // 해당 문자열 전체를 사용할 경우 nullptr입력, 아니라면 길이 명시
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

	float positions[6] = { //삼각형 좌표 정보. 현재는 CPU side memory에 있음
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f,
	};

	//---------데이터를 전달하는 과정--------//
	unsigned int bufferID;
	glGenBuffers(1, &bufferID); //1. 버퍼 생성
	glBindBuffer(GL_ARRAY_BUFFER, bufferID); //2. 바인딩("작업 상태")
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);  //3. 작업 상태 버퍼에 데이터 전달

	//---------데이터를 해석하는(법을 정의하는) 과정--------//
	glEnableVertexAttribArray(0); //1. 몇 번째 Location의 attribute를 활성화(enable)
	glVertexAttribPointer(0, 2,	GL_FLOAT, GL_FALSE, sizeof(float)*2, 0); //2. 데이터 해석 방법을 전달.

	//---------Shader 생성---------------//
	std::string vertexShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec4 position;" //여기 있는 location = 0가, 118, 119 line의 0을 의미함
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = position;\n" //119에서 보다시피, 2개의 값만 전달했지만, 알아서 vec4로 변환해줌
		"}\n";

	std::string fragShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) out vec4 color;" //출력 color
		"\n"
		"void main()\n"
		"{\n"
		"	color = vec4(1.0, 1.0 ,0.0, 1.0);\n" //빨간색 반환
		"}\n";

	unsigned int shader = CreateShader(vertexShader, fragShader);
	glUseProgram(shader); //BindBuffer와 마찬가지로, 현재 셰이더 프로그램을 "작업 상태"로 놓음
						  //draw call은 작업 상태인 셰이더 프로그램을 사용하여 작업 상태인 버퍼 데이터를 그림

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3); //Draw call
		
		//이번에는 빨간색 삼각형이 나와야 함!

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader); //셰이더 삭제

	glfwTerminate();
	return 0;
}