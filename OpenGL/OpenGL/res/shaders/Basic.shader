#shader vertex
#version 330

//Model Ŭ������ ���� 3���� ��ǥ, 2���� �ؽ�ó ��ǥ, 3���� ���� ���� �����͸� �Ѱ��ִ� ���� �� �� ����
//�̷� ������ �Ѿ������ �츮�� vertex buffer layout�� ���� ��������
layout(location = 0) in vec3 position; 
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec2 v_TexCoord; //���ؽ� ���̴����� �߰������� ����ϴ� ������(varying)
out vec3 v_Normal;

uniform mat4 u_Model; //world ��ȯ ���
uniform mat4 u_View; //ī�޶� ���� ��ȭ�� View ���
uniform mat4 u_Projection; //CPU���� ���޵� (glm ���̺귯���� ���� ������) ���� ���

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(position,1.0); // ���� ��ġ�� ���� ��İ� ��
	v_TexCoord = texCoord; //vertex ��ȯ�� ���� �ٲ��� �����Ƿ� �״�� �Ѱ��ָ� ��
	v_Normal = normal;
};

#shader fragment
#version 330

layout(location = 0) out vec4 color;

in vec2 v_TexCoord; //���ؽ� ���̴����� �Ѱܹ��� ������
in vec3 v_Normal;

uniform sampler2D u_Texture; //texture�� sampler2D Ÿ��

void main()
{
	//--���������� ǥ���� ����
	//color = vec4(1.0, 0.0, 0.0, 1.0);

	//--normal�� �������� ǥ���� ����
	vec3 normalColor = (v_Normal + 1.0) / 2.0;
	color = vec4(normalColor, 1.0);

	//--uvchecker ���� ����, �� � �κ��� ���������� ���ϱ�?
	color = texture(u_Texture, v_TexCoord);

	//color = v_Color;
};