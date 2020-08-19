#shader vertex
#version 330

layout(location = 0) in vec3 position;

uniform mat4 u_Model;
uniform mat4 u_DirectionalLightTransform; //ī�޶� directional light ��ġ�� ���⿡ �ִٰ� ������ VP ���

void main()
{
	gl_Position = u_DirectionalLightTransform * u_Model * vec4(position, 1.0);
	//gl_Position = vec4(0.0, 0.0, 1000.0, 1.0);
}

#shader fragment
#version 330

//�����׸�Ʈ ���̴��� �ƹ��͵� ���� ����. depth ���� ����� ���̱� ������

void main()
{
}