#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// �̹����� Vertex Array�� Shader�� �̿��Ͽ� �ﰢ���� �׸��� Modern OpenGL ������� ������ ����
// Vertex Array�� GPU�� VRAM�� Buffer�� ����Ǵ� �����͸� �ѱ�� ����� �̾߱���
// �����͸� �ѱ�� ���� �ﰢ���� ������ �׸��� ����� ȣ���ϴ� ���� Draw call�̶�� ��
// �ﰢ���� ��� �׷����� ����ڰ� ������ ���α׷��� Shader��

// OpenGL�� State Machine�� �����. �Ϲ����� ��ü ���� ���α׷��� ����ʹ� �ٸ���,
// ���� ó���� �����͸� ������ ��, ���� �����ؾ� �� �۾��� �Լ� ȣ���� ���� ó���ϴ� �����
// �����ϰ� �� �ﰢ���� ȭ�鿡 �׸��� �ǻ��ڵ�� �����ϸ�,

// ---��ü���� ���
// Triangle t1, t2; //�ﰢ�� �� ���� ����
// Draw(t1); //t1 ��ü�� ���������ν� �ﰢ�� 1�� �׸�
// Draw(t2); //t2 ��ü�� ���������ν� �ﰢ�� 2�� �׸�

// ---State Machine ���
// Triangle t1, t2; //�ﰢ�� �� ���� ����
// Activate(t1); //�ﰢ�� 1�� ó������ ���·� ����
// Draw(); //���� ó������ ������(=�ﰢ�� 1)�� ȭ�鿡 �׸�
// Activate(t2); //�ﰢ�� 2�� ó������ ���·� ����
// Draw(); //���� ó������ ������(=�ﰢ�� 2)�� ȭ�鿡 �׸�

//--------Shader ������ �Լ�----------//
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type); //���̴� ��ü ����(��������)
	const char* src = source.c_str();
	glShaderSource(id, // ���̴��� �ҽ� �ڵ� ���, �ҽ� �ڵ带 ����� ���̴� ��ü id
					1, // �� ���� �ҽ� �ڵ带 ����� ������
					&src, // ���� �ҽ� �ڵ尡 ����ִ� ���ڿ��� �ּҰ�
					nullptr); // �ش� ���ڿ� ��ü�� ����� ��� nullptr�Է�, �ƴ϶�� ���� ���
	glCompileShader(id); // id�� �ش��ϴ� ���̴� ������
	
	// Error Handling(������ ���̴� ���α׷����Ҷ� ���Ӵ�...)
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result); //���̴� ���α׷����κ��� ������ ���(log)�� ����
	if (result == GL_FALSE) //�����Ͽ� ������ ���
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); //log�� ���̸� ����
		char* message = (char*)alloca(length * sizeof(char)); //stack�� �����Ҵ�
		glGetShaderInfoLog(id, length, &length, message); //���̸�ŭ log�� ����
		std::cout << "���̴� ������ ����! " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id); //������ ������ ��� ���̴� ����
		return 0;
	}

	return id;
}

//--------Shader ���α׷� ����, ������, ��ũ----------//
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragShader)
{
	unsigned int program = glCreateProgram(); //���̴� ���α׷� ��ü ����(int�� ����Ǵ� ���� id)
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader); 
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragShader);

	//�����ϵ� ���̴� �ڵ带 program�� �߰��ϰ� ��ũ
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	//���̴� ���α׷��� ���������Ƿ� vs, fs ���� ���α׷��� ���̻� �ʿ� ����
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

	// glfwMakeContextCurrent�� ȣ��� �Ŀ� glewInit�� ����Ǿ�� ��
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error\n";
	}

	std::cout << glGetString(GL_VERSION) << std::endl; //�� �÷����� GL_Version ����غ���

	//-----Modern OpenGL ���--------//
	// http://docs.gl/ ���� �Ʒ� gl�Լ����� reference�� ã�� �� ����

	float positions[6] = { //�ﰢ�� ��ǥ ����. ����� CPU side memory�� ����
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f,
	};

	//---------�����͸� �����ϴ� ����--------//
	unsigned int bufferID;
	glGenBuffers(1, &bufferID); //1. ���� ����
	glBindBuffer(GL_ARRAY_BUFFER, bufferID); //2. ���ε�("�۾� ����")
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);  //3. �۾� ���� ���ۿ� ������ ����

	//---------�����͸� �ؼ��ϴ�(���� �����ϴ�) ����--------//
	glEnableVertexAttribArray(0); //1. �� ��° Location�� attribute�� Ȱ��ȭ(enable)
	glVertexAttribPointer(0, 2,	GL_FLOAT, GL_FALSE, sizeof(float)*2, 0); //2. ������ �ؼ� ����� ����.

	//---------Shader ����---------------//
	std::string vertexShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec4 position;" //���� �ִ� location = 0��, 118, 119 line�� 0�� �ǹ���
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = position;\n" //119���� ���ٽ���, 2���� ���� ����������, �˾Ƽ� vec4�� ��ȯ����
		"}\n";

	std::string fragShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) out vec4 color;" //��� color
		"\n"
		"void main()\n"
		"{\n"
		"	color = vec4(1.0, 1.0 ,0.0, 1.0);\n" //������ ��ȯ
		"}\n";

	unsigned int shader = CreateShader(vertexShader, fragShader);
	glUseProgram(shader); //BindBuffer�� ����������, ���� ���̴� ���α׷��� "�۾� ����"�� ����
						  //draw call�� �۾� ������ ���̴� ���α׷��� ����Ͽ� �۾� ������ ���� �����͸� �׸�

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3); //Draw call
		
		//�̹����� ������ �ﰢ���� ���;� ��!

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader); //���̴� ����

	glfwTerminate();
	return 0;
}