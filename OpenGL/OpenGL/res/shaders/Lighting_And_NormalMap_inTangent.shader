#shader vertex
#version 330

//Model Ŭ������ ���� 3���� ��ǥ, 2���� �ؽ�ó ��ǥ, 3���� ���� ���� �����͸� �Ѱ��ִ� ���� �� �� ����
//�̷� ������ �Ѿ������ �츮�� vertex buffer layout�� ���� ��������
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

out vec2 v_TexCoord; //���ؽ� ���̴����� �߰������� ����ϴ� ������(varying)
out vec3 v_Normal;
out vec3 v_WorldPosition;
out mat3 v_TBN;

uniform mat4 u_Model; //world ��ȯ ���
uniform mat4 u_View; //ī�޶� ���� ��ȭ�� View ���
uniform mat4 u_Projection; //CPU���� ���޵� (glm ���̺귯���� ���� ������) ���� ���

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0); // ���� ��ġ�� ���� ��İ� ��
	v_TexCoord = texCoord; //vertex ��ȯ�� ���� �ٲ��� �����Ƿ� �״�� �Ѱ��ָ� ��

	mat3 ivtr = transpose(inverse(mat3(u_Model)));
	v_Normal = normalize(ivtr * normal);
	vec3 tan = normalize(ivtr * tangent);
	vec3 bitan = cross(v_Normal, tan);

	v_WorldPosition = (u_Model * vec4(position, 1.0)).xyz; //������ World space ��ǥ
	

	//tangent space ��ȯ ����
	v_TBN = transpose(mat3(tan, bitan, v_Normal));
};

#shader fragment
#version 330

layout(location = 0) out vec4 color;

//���̴������� Composition�� ���� ���� ����
struct Light
{
	vec3 lightColor;
	float ambientIntensity;
	float diffuseIntensity;
};

//���̴� �ڵ忡�� ����ü ��� ����
struct DirectionalLight
{
	Light base;
	vec3 direction;
};

struct Material
{
	float specularIntensity;
	float shininess; //sh
};

in vec2 v_TexCoord; //���ؽ� ���̴����� �Ѱܹ��� ������
in vec3 v_Normal;
in vec3 v_WorldPosition;
in mat3 v_TBN;

uniform vec3 u_EyePosition; //specular ����� ���� ī�޶� ��ġ
uniform sampler2D u_Texture; //texture�� sampler2D Ÿ��
uniform DirectionalLight u_DirectionalLight;
uniform Material u_Material;

uniform sampler2D u_Normal; //normal map texture �߰�

vec3 ToTangenSpaceVector(vec3 dir)
{
	return (v_TBN * dir);
}

//�� �⺻ ������ ������ �־����� ��, rendering eqn�� ���� ���� ���
vec3 CalcLight(Light light, vec3 direction)
{
	//vec3 normal = normalize(v_Normal); //���� ���� normalize �ʿ�! (���� ��� 76 line���� �߸� ����)
	
	//normap map �ؽ�ó�� ������, normal�� ���� ���� �ƴ϶� texture���� ���ø��� ���� ���!
	vec3 normal = 2.0 * texture(u_Normal, v_TexCoord).rgb - 1.0; //rgb���� ������ xyz component
	normal = normalize(normal);
	
	vec3 lightAmbient = light.lightColor * light.ambientIntensity;
	
	vec3 lightDir = normalize(-ToTangenSpaceVector(direction)); //light ������ tangent space�� ��ȯ
	float diffuseFactor = max(dot(normal, lightDir), 0.0);
	vec3 lightDiffuse = light.lightColor * light.diffuseIntensity * diffuseFactor;

	vec3 fragToEye = normalize(ToTangenSpaceVector(u_EyePosition - v_WorldPosition)); //view ������ tangent space�� ��ȯ
	vec3 rVec = 2.0 * normal * dot(normal, lightDir) - lightDir; //r vector ���
	vec3 lightSpecular = pow(max(dot(rVec, fragToEye), 0.0), u_Material.shininess) * light.lightColor * u_Material.specularIntensity;

	return (lightAmbient + lightDiffuse + lightSpecular); //light�� direction�� ���� light color ��ȯ
}

//Directional Light�� ��� ���� ���
vec3 CalcDirectionalLight()
{
	return CalcLight(u_DirectionalLight.base, u_DirectionalLight.direction);
}

void main()
{
	vec3 lightColor = CalcDirectionalLight();
	color = texture(u_Texture, v_TexCoord) * vec4(lightColor, 1.0);
};