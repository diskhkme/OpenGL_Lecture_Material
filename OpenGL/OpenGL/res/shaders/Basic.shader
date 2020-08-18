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

//���̴� �ڵ忡�� ����ü ��� ����
struct DirectionalLight
{
	//���� �ڷῡ�� s_a�� �ش� (acolor * aintensity = s_a)
	vec3 lightColor; 
	float ambientIntensity;
};

//-- �Ʒ��� ���� ���ϰ� Material�� �Ӽ��� color�� ���ϴ� ��쵵 ������,
//-- ��ü�� diffuse color�� ambient/specular �÷��� ũ�� �ٸ��� �ʴٰ� �����Ͽ� ����ȭ ����
//-- ��, m_a = m_d & m_s = m_d �� ����
//-- m_d��? texture color

//struct Material {
//	vec3 ambient; //m_a
//	vec3 diffuse; //m_d
//	vec3 specular; //m_s
//	float shininess; //sh
//};
//uniform Material u_Material;

in vec2 v_TexCoord; //���ؽ� ���̴����� �Ѱܹ��� ������
in vec3 v_Normal;

uniform sampler2D u_Texture; //texture�� sampler2D Ÿ��
uniform DirectionalLight u_DirectionalLight;

void main()
{
	vec3 lightAmbient = u_DirectionalLight.lightColor * u_DirectionalLight.ambientIntensity;
	color = texture(u_Texture, v_TexCoord) * vec4(lightAmbient,1.0);
	//color = vec4(1.0, 0.0, 0.0, 1.0);
};