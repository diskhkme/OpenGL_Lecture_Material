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
	// 위에서는 Byte의 덩어리를 GPU로 전달했을 뿐, 그 데이터를 어떻게 나누어 사용할지는 알 수 없음
	// 어떻게 나누어서 사용해야 하는지를 아래 두 과정을 통해 알려주어야 함
	// 아래 "몇 번째 location"은 Shader를 보면 분명해질 것임

	glEnableVertexAttribArray(0); //1. 몇 번째 Location의 attribute를 활성화(enable)
	glVertexAttribPointer(0, //2. 데이터 해석 방법을 전달. 몇 번째 location의 attribute의 데이터 해석 방법인지
						2,//각 데이터가 몇 개 단위로 이루어져 있는지(현재 예제에서 각 점은 두 개의 float으로 표현되므로 2임)
						GL_FLOAT, //데이터 타입
						GL_FALSE, //정규화가 필요한지
						sizeof(float)*2, //한 단위의 데이터를 읽을 때마다, 얼마나 건너뛰어야(stride) 하는지(=첫 데이터와 두 번째 데이터가 얼마나 떨어져있는지)
						0); //첫 데이터가 몇 바이트부터 시작하는지(복잡한 상황에 대한 그림은 강의자료 참고)

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3); //Draw call
		
		//Attribute를 추가했더니 화면에 흰색 삼각형이 나옴!(GPU마다 다를 수 있음)
		//사실은 셰이더가 없기 때문에 그려지지 않는 것이 맞지만, 
		//GPU 드라이버에서 셰이더가 없을 때 흰색으로 그리는 등의 예외 처리를 해 두었기 때문에 화면에 나오는 것임

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}