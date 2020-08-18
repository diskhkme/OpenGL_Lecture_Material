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

	//https://www.youtube.com/watch?v=d3EUd2HxsO4&app=desktop
	v_Normal = mat3(transpose(inverse(u_Model))) * normal;
};

#shader fragment
#version 330

layout(location = 0) out vec4 color;

//���̴� �ڵ忡�� ����ü ��� ����
struct DirectionalLight
{
	vec3 lightColor;
	//for ambient
	float ambientIntensity;
	//for diffuse 
	vec3 direction;
	float diffuseIntensity;
};

in vec2 v_TexCoord; //���ؽ� ���̴����� �Ѱܹ��� ������
in vec3 v_Normal;

uniform sampler2D u_Texture; //texture�� sampler2D Ÿ��
uniform DirectionalLight u_DirectionalLight;

void main()
{
	vec3 lightAmbient = u_DirectionalLight.lightColor * u_DirectionalLight.ambientIntensity;

	//���� ������ ���ؼ� if �б⹮�� ������ �ʴ� ���� �ʿ�! (��� max �Լ� ���)
	//light ������ ������ �Ϳ� ����
	//normal�� ���� ���ͷ� �Ѿ������, scan conversion���� �����Ǿ� ���̰� ���� �� �����Ƿ� �ٽ� normalization �ʿ�!
	float diffuseFactor = max(dot(normalize(v_Normal), normalize(-u_DirectionalLight.direction)), 0.0);
	vec3 lightDiffuse = u_DirectionalLight.lightColor * u_DirectionalLight.diffuseIntensity * diffuseFactor;

	color = texture(u_Texture, v_TexCoord) * vec4(lightAmbient + lightDiffuse,1.0);
	//color = vec4(1.0, 0.0, 0.0, 1.0);
};