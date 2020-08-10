#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

//__debugbreak()는 MSVC에만 사용 가능
#define ASSERT(x) if ((!x)) __debugbreak(); 
// GLCall로 감싸서 gl 함수를 호출하면 오류 라인과 메시지 확인 가능
#define GLCall(x) GLClearError();\
				  x;\
				  ASSERT(GLLogCall(#x, __FILE__, __LINE__))

//glGetError는 에러를 하나씩만 반환하기 때문에, 한 번 확인에 모든 오류를 뽑아내는 것이 필요함
static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR); // GL_NO_ERROR == 0
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << ") : " << function <<
			" " << file << " in line " << line << std::endl;
		return false;
	}
	return true;
}

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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Opengl 3.3 버전 사용
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); //Compatability 버전일때는 VAO를 안만들어도 동작
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1); //1이면 vsync rate와 같은 속도로 화면 갱신

	// glfwMakeContextCurrent가 호출된 후에 glewInit이 수행되어야 함
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error\n";
	}

	std::cout << glGetString(GL_VERSION) << std::endl; //내 플랫폼의 GL_Version 출력해보기

	//-----Modern OpenGL 방식--------//
	// http://docs.gl/ 에서 아래 gl함수들의 reference를 찾을 수 있음

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

	//loop에 적어놓은 반복적 작업을 한 그룹으로 묶기 위해 VAO(Vertex Array Object)를 사용해야 함
	//VAO는 아래 데이터 전달 과정 및 해석 과정을 담고있는 데이터라고 생각하면 됨
	unsigned int vao;
	glGenVertexArrays(1, &vao); //vao 생성
	glBindVertexArray(vao); //vao 바인딩(="작업 상태")

	//---------데이터를 전달하는 과정--------//
	unsigned int vbo; //개념적으로 buffer ID라 불렀고, 실제로는 vbo(vertex buffer object)라는 용어를 많이 씀
	glGenBuffers(1, &vbo); //1. 버퍼 생성
	glBindBuffer(GL_ARRAY_BUFFER, vbo); //2. 바인딩("작업 상태")
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positions, GL_STATIC_DRAW);  //3. 작업 상태 버퍼에 데이터 전달

	//---------데이터를 해석하는(법을 정의하는) 과정--------//
	glEnableVertexAttribArray(0); //1. 몇 번째 Location의 attribute를 활성화(enable)
	glVertexAttribPointer(0, 2,	GL_FLOAT, GL_FALSE, sizeof(float)*2, 0); //2. 데이터 해석 방법을 전달. (실제로 VAO에 정보가 저장되는 시점)

	unsigned int ibo; //index buffer object
	glGenBuffers(1, &ibo); //1. 인덱스 버퍼 생성
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); //2. 바인딩("작업 상태"), 이번에는 ARRAY_BUFFER가 아니라 ELEMENT_ARRAY_BUFFER
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);  //3. 작업 상태 버퍼에 데이터 전달
	   
	//---------Shader 생성---------------//
	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader"); //셰이더 코드 파싱

	unsigned int shader = CreateShader(source.VertexSource, source.FragSource);
	glUseProgram(shader); //BindBuffer와 마찬가지로, 현재 셰이더 프로그램을 "작업 상태"로 놓음
						  //draw call은 작업 상태인 셰이더 프로그램을 사용하여 작업 상태인 버퍼 데이터를 그림

	//--------Uniform 데이터 전달--------//
	// *주의* 이 부분도 당연히 shader가 바인딩("작업 상태")된 상태에서 수행해야 함
	GLCall(int location = glGetUniformLocation(shader, "u_Color")); //셰이더 프로그램에 uniform 데이터를 전달하기 위해서는 uniform의 위치 정보가 필요. 
															//셰이더 객체와 변수 이름을 인자로 넘겨주어 얻어올 수 있음
	ASSERT(location != 1);
	GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

	glBindVertexArray(0); //vao unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0); //객체 인자를 0으로 바인딩하면, 현재 작업 상태를 해제한다는 의미(unbind)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //객체 인자를 0으로 바인딩하면, 현재 작업 상태를 해제한다는 의미(unbind)
	glUseProgram(0); //객체 인자를 0으로 바인딩하면, 현재 작업 상태를 해제한다는 의미(unbind)


	float r = 0.0f;
	float increment = 0.05f;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		//https://webgl2fundamentals.org/webgl/lessons/resources/webgl-state-diagram.html?exampleId=triangle#no-help
		//https://www.youtube.com/watch?v=WMiggUPst-Q
		//위(line 201~204)에서 unbind한 상태로 다시 바인딩 없이 물체를 그리면 오류가 발생함. (현재 "작업 상태"인 데이터가 없기 때문에 그릴 것이 없음)
		//이전에는 loop 밖에서 바인딩을 한 상태 그대로 계속 그렸기 때문에 문제없었음.
		//문제는, 여러 물체를 여러 셰이더를 사용해 그리려면 한 프레임 그릴 때마다 아래와 같은 코드를 반복적으로 호출해야 한다는 것임
		//이러한 문제를 해결하기 위해서, VAO를 사용함(사용 해야 함)
		//---------물체 하나를 그리기 위해 draw 전에 해야 하는 작업들(vao 정의를 사용하지 않을때)-----------//
		//// 1. 셰이더 바인딩, uniform 데이터 전달
		//glUseProgram(shader);
		//GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
		//// 2. vertex array 바인딩, 데이터 읽는 방법 전달
		//glBindBuffer(GL_ARRAY_BUFFER, vbo); 
		//glEnableVertexAttribArray(0); //1. 몇 번째 Location의 attribute를 활성화(enable)
		//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0); //2. 데이터 해석 방법을 전달.
		//// 3. index array 바인딩
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		//-------------------------------------------------------------------//

		//---------물체 하나를 그리기 위해 draw 전에 해야 하는 작업들(vao 정의를 사용할 때)-----------//
		//1. 셰이더 바인딩, uniform 데이터 전달
		GLCall(glUseProgram(shader));
		GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
		// 2. VAO 바인딩(데이터 전달과 읽는 방법, index가 저장되어 있음)
		GLCall(glBindVertexArray(vao));
		//-------------------------------------------------------------------//
		
		GLCall(glDrawElements(GL_TRIANGLES, 6,	GL_UNSIGNED_INT, nullptr)); //Draw call
		
		if (r > 1.0f)
			increment = -0.05f;
		if (r < 0.0f)
			increment = 0.05f;

		r += increment;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader); //셰이더 삭제

	glfwTerminate();
	return 0;
}