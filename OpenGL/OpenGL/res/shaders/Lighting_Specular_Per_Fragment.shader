#shader vertex
#version 330

//Model Ŭ������ ���� 3���� ��ǥ, 2���� �ؽ�ó ��ǥ, 3���� ���� ���� �����͸� �Ѱ��ִ� ���� �� �� ����
//�̷� ������ �Ѿ������ �츮�� vertex buffer layout�� ���� ��������
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec2 v_TexCoord; //���ؽ� ���̴����� �߰������� ����ϴ� ������(varying)
out vec3 v_Normal;
out vec3 v_WorldPosition;
out vec4 v_DirectionalLightSpacePos; //Light ������ fragment position

uniform mat4 u_Model; //world ��ȯ ���
uniform mat4 u_View; //ī�޶� ���� ��ȭ�� View ���
uniform mat4 u_Projection; //CPU���� ���޵� (glm ���̺귯���� ���� ������) ���� ���
uniform mat4 u_DirectionalLightTransform;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0); // ���� ��ġ�� ���� ��İ� ��
	v_TexCoord = texCoord; //vertex ��ȯ�� ���� �ٲ��� �����Ƿ� �״�� �Ѱ��ָ� ��

	v_Normal = mat3(transpose(inverse(u_Model))) * normal;

	v_WorldPosition = (u_Model * vec4(position, 1.0)).xyz; //������ World space ��ǥ

	//�� ���̴��� u_View�� ī�޶� ������ view �����
	//�׸��� ����� ���ؼ��� �ش� fragment�� ī�޶� �������� ��� ��ġ���� �˾ƾ� �ϱ� ������
	//�Ʒ��� ���� ����Ͽ� varying���� fragment�� �ѱ�
	v_DirectionalLightSpacePos = u_DirectionalLightTransform * u_Model*vec4(position, 1.0);
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
in vec4 v_DirectionalLightSpacePos; //Light ������ fragment position

uniform vec3 u_EyePosition; //specular ����� ���� ī�޶� ��ġ
uniform sampler2D u_Texture; //texture�� sampler2D Ÿ��
uniform DirectionalLight u_DirectionalLight;
uniform Material u_Material;
uniform sampler2D u_DirectionalShadowMap; //shadow map �ؽ�ó

float CalcDirectionalShadowFactor(DirectionalLight light)
{
	//�� ������ perspective division�� �ڵ����� ���� �ʱ⶧���� ���� ���־�� ��
	vec3 projCoords = v_DirectionalLightSpacePos.xyz / v_DirectionalLightSpacePos.w;
	projCoords = (projCoords * 0.5) + 0.5; //[-1,1] (NDC) -> [0,1] (Depth) ��ȯ

	float closest = texture(u_DirectionalShadowMap, projCoords.xy).r; //shadow map �ؽ�ó�� ����� ���� ���� ���ø�
	float current = projCoords.z; //���� frag�� light���� ����

	//������ ������ bias ��� ����� ���� ������ bias ����
	vec3 normal = normalize(v_Normal);
	vec3 lightDir = normalize(light.direction);
	float bias = max(0.05*(1.0 - dot(normal, lightDir)),0.055);

	//Percentage close filtering
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(u_DirectionalShadowMap, 0); //�ؽ�ó ũ���� ����, �Ʒ� loop���� 1,0,-1�� uv coord ������ ��ȯ�ϱ� ���� ���
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			float pcfDepth = texture(u_DirectionalShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += current - bias > pcfDepth ? 1.0 : 0.0; //aggregation
		}
	}
	shadow /= 9; //��հ� ���

	if (projCoords.z > 1.0) //far plane �������δ� shadow ���� ����
	{
		shadow = 0.0;
	}

	return shadow;
}

//�� �⺻ ������ ������ �־����� ��, rendering eqn�� ���� ���� ���
vec3 CalcLight(Light light, vec3 direction, float shadowFactor) //shadow Factor �߰�
{
	vec3 lightAmbient = light.lightColor * light.ambientIntensity;
	vec3 normal = normalize(v_Normal); //(����!)���� ���� ����ȭ �ؾ� ��!

	vec3 lightDir = normalize(-direction);
	float diffuseFactor = max(dot(normal, lightDir), 0.0);
	vec3 lightDiffuse = light.lightColor * light.diffuseIntensity * diffuseFactor;

	vec3 fragToEye = normalize(u_EyePosition - v_WorldPosition);
	vec3 rVec = 2.0 * normal * dot(normal, lightDir) - lightDir; //r vector ���
	vec3 lightSpecular = pow(max(dot(rVec, fragToEye), 0.0), u_Material.shininess) * light.lightColor * u_Material.specularIntensity;

	//shadow factor�� 1�̸� diffuse�� specular ���� ����(ambient�� ���� �����̹Ƿ� �׸��ڿ��� ����)
	return (lightAmbient + (1.0-shadowFactor)*(lightDiffuse + lightSpecular)); //light�� direction�� ���� light color ��ȯ
}

//Directional Light�� ��� ���� ���
vec3 CalcDirectionalLight()
{
	float shadowFactor = CalcDirectionalShadowFactor(u_DirectionalLight);
	return CalcLight(u_DirectionalLight.base, u_DirectionalLight.direction, shadowFactor);
}

void main()
{
	
	vec3 lightColor = CalcDirectionalLight();
	color = texture(u_Texture, v_TexCoord) * vec4(lightColor, 1.0);
};