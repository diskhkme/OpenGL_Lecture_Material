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
	// �������� Byte�� ����� GPU�� �������� ��, �� �����͸� ��� ������ ��������� �� �� ����
	// ��� ����� ����ؾ� �ϴ����� �Ʒ� �� ������ ���� �˷��־�� ��
	// �Ʒ� "�� ��° location"�� Shader�� ���� �и����� ����

	glEnableVertexAttribArray(0); //1. �� ��° Location�� attribute�� Ȱ��ȭ(enable)
	glVertexAttribPointer(0, //2. ������ �ؼ� ����� ����. �� ��° location�� attribute�� ������ �ؼ� �������
						2,//�� �����Ͱ� �� �� ������ �̷���� �ִ���(���� �������� �� ���� �� ���� float���� ǥ���ǹǷ� 2��)
						GL_FLOAT, //������ Ÿ��
						GL_FALSE, //����ȭ�� �ʿ�����
						sizeof(float)*2, //�� ������ �����͸� ���� ������, �󸶳� �ǳʶپ��(stride) �ϴ���(=ù �����Ϳ� �� ��° �����Ͱ� �󸶳� �������ִ���)
						0); //ù �����Ͱ� �� ����Ʈ���� �����ϴ���(������ ��Ȳ�� ���� �׸��� �����ڷ� ����)

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3); //Draw call
		
		//Attribute�� �߰��ߴ��� ȭ�鿡 ��� �ﰢ���� ����!(GPU���� �ٸ� �� ����)
		//����� ���̴��� ���� ������ �׷����� �ʴ� ���� ������, 
		//GPU ����̹����� ���̴��� ���� �� ������� �׸��� ���� ���� ó���� �� �ξ��� ������ ȭ�鿡 ������ ����

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}