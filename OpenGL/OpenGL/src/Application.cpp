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
	glGenBuffers(1, &bufferID); //1. �����͸� ������ ���� ��ü�� �����(1��), �� ���� ��ü�� �ּҸ� bufferID�� ����
	glBindBuffer(GL_ARRAY_BUFFER, bufferID); //2. ��� ���� buffer(�� ��° ���ڷ� �ּҸ� �Ѱ���)�� "�۾� ����"�� ����
											// ù ��° GL_ARRAY_BUFFER�� ���ۿ� �迭 �����Ͱ� ����� ���̶�� �ǹ�
	glBufferData(GL_ARRAY_BUFFER, //3. ���� ������ GPU�� �����͸� �Ѱ��ִ� �Լ��� ȣ�� 
				6 * sizeof(float), //�������� ũ�⸦ ����
				positions,		  //������ ������ ����
				GL_STATIC_DRAW);   //������ ������ ���� ���̶�� ���� �˷���(GPU�� ȿ������ ������ ���� Hint�� ��)

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		// �ﰢ�� �׸��� Legacy �ڵ� �߰� <-- ���� �����.
		//glBegin(GL_TRIANGLES);
		//glVertex2f(-0.5f, -0.5f);
		//glVertex2f( 0.0f,  0.5f);
		//glVertex2f( 0.5f, -0.5f);
		//glEnd();

		//----------Modern OpenGL----------//
		// �����ؼ� �˾ƾ� �� �� : �Ʒ� draw call����, �ﰢ���� �׸��� ����
		// 66��° line���� glBindBuffer()�� �Ǿ� �ֱ� ������.
		// �Ʒ� glDrawArray������ ������ �׸� ������ ���ڷ� �ѱ�� ���� �ƴ϶�,
		// ���� "�۾� ����"�� ���� �ִ� ����� �׸��� ��
		glDrawArrays(GL_TRIANGLES, //���� draw call, �ﰢ���� �׸� ���̶�� ���
					0,				//�� ��° �����ͺ��� �׸������� ���(��� �׸��ٸ� 0)
					3);				//�� ���� �����͸� �׸� ������ ���
		
		//����� �ƹ��͵� ȭ�鿡 ������ ���� ����. 
		//�ֳ��ϸ� �ﰢ���� ��� �׸� ������ ���̴��� ���� �˷����� �ʾұ� ����!

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}