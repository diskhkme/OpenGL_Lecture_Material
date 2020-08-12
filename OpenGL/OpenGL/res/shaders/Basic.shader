#shader vertex
#version 330

layout(location = 0) in vec4 position; 
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord; //���ؽ� ���̴����� �߰������� ����ϴ� ������(varying)

uniform mat4 u_Projection; //CPU���� ���޵� (glm ���̺귯���� ���� ������) ���� ���

void main()
{
	gl_Position = u_Projection * position; // ���� ��ġ�� ���� ��İ� ��
	v_TexCoord = texCoord; //vertex ��ȯ�� ���� �ٲ��� �����Ƿ� �״�� �Ѱ��ָ� ��
};

#shader fragment
#version 330

layout(location = 0) out vec4 color;

in vec2 v_TexCoord; //���ؽ� ���̴����� �Ѱܹ��� ������

uniform vec4 u_Color; //uniform���� id�� �Ҵ��
uniform sampler2D u_Texture; //texture�� sampler2D Ÿ��

void main()
{
	vec4 texColor = texture(u_Texture, v_TexCoord); //texture �Լ��� texcoord��ġ�� �ȼ� ���� ���� ���ǵ� ����� ���� ���ø�
	color = texColor; //���� �ؽ�ó �̹������� ���ø��� ������ fragment �������� ����
};