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

//���̴� ���� �Ľ� �Լ�
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
			if (line.find("vertex") != std::string::npos) //vertex ���̴� ����
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) //fragment ���̴� ����
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n'; //�ڵ带 stringstream�� ����
		}
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type); //���̴� ��ü ����(��������)
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr); // ���̴��� �ҽ� �ڵ� ���
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

	//float positions[] = { //�簢���� �׸��� ���� 1�� ����
	//	-0.5f, -0.5f,
	//	 0.5f, -0.5f,
	//	 0.5f,  0.5f,

	//	 0.5f,  0.5f, //������ �ߺ�
	//	-0.5f,  0.5f,
	//	-0.5f, -0.5f, //������ �ߺ�, ����� ��ġ ������ ��� ������ �Ϲ������� ���� �� ���� ������ �����Ƿ�, ���� ŭ
	//};

	float positions[] = { //�簢���� �׸��� ���� 2�� ����
		-0.5f, -0.5f, //0
		 0.5f, -0.5f, //1
		 0.5f,  0.5f, //2
		-0.5f,  0.5f, //3
	};

	unsigned int indices[] = { //index buffer�� �Բ� ���(index�� unsigned Ÿ���ӿ� ����)
		0, 1, 2, //vertex 012�� �̷���� �ﰢ��
		2, 3, 0  //vertex 230�� �̷���� �ﰢ��
	};

	//---------�����͸� �����ϴ� ����--------//
	unsigned int bufferID;
	glGenBuffers(1, &bufferID); //1. ���� ����
	glBindBuffer(GL_ARRAY_BUFFER, bufferID); //2. ���ε�("�۾� ����")
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positions, GL_STATIC_DRAW);  //3. �۾� ���� ���ۿ� ������ ����

	//---------�����͸� �ؼ��ϴ�(���� �����ϴ�) ����--------//
	glEnableVertexAttribArray(0); //1. �� ��° Location�� attribute�� Ȱ��ȭ(enable)
	glVertexAttribPointer(0, 2,	GL_FLOAT, GL_FALSE, sizeof(float)*2, 0); //2. ������ �ؼ� ����� ����.

	unsigned int ibo; //index buffer object
	glGenBuffers(1, &ibo); //1. �ε��� ���� ����
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); //2. ���ε�("�۾� ����"), �̹����� ARRAY_BUFFER�� �ƴ϶� ELEMENT_ARRAY_BUFFER
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);  //3. �۾� ���� ���ۿ� ������ ����
	   
	//---------Shader ����---------------//
	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader"); //���̴� �ڵ� �Ľ�

	unsigned int shader = CreateShader(source.VertexSource, source.FragSource);
	glUseProgram(shader); //BindBuffer�� ����������, ���� ���̴� ���α׷��� "�۾� ����"�� ����
						  //draw call�� �۾� ������ ���̴� ���α׷��� ����Ͽ� �۾� ������ ���� �����͸� �׸�

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		//glDrawArrays(GL_TRIANGLES, 0, 6); //Draw call
		glDrawElements(GL_TRIANGLES, //index buffer�� ����� ���� drawelement�Լ��� ����ؾ� ��
						6,			//�׸� index�� ����
						GL_UNSIGNED_INT, //index Ÿ�� (GL_INT���� �߸����� �ȵ�!)
						nullptr); //offset ������
		

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader); //���̴� ����

	glfwTerminate();
	return 0;
}