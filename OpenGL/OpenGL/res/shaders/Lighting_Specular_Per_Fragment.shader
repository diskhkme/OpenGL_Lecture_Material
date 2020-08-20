#shader vertex
#version 330

//Model 클래스를 보면 3개의 좌표, 2개의 텍스처 좌표, 3개의 법선 벡터 데이터를 넘겨주는 것을 알 수 있음
//이런 순서로 넘어오도록 우리가 vertex buffer layout을 통해 정의했음
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec2 v_TexCoord; //버텍스 셰이더에서 추가적으로 출력하는 데이터(varying)
out vec3 v_Normal;
out vec3 v_WorldPosition;
out vec4 v_DirectionalLightSpacePos; //Light 기준의 fragment position

uniform mat4 u_Model; //world 변환 행렬
uniform mat4 u_View; //카메라를 통해 변화된 View 행렬
uniform mat4 u_Projection; //CPU에서 전달될 (glm 라이브러리를 통해 생성된) 투영 행렬
uniform mat4 u_DirectionalLightTransform;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0); // 정점 위치를 투영 행렬과 곱
	v_TexCoord = texCoord; //vertex 변환에 따라 바뀌지 않으므로 그대로 넘겨주면 됨

	v_Normal = mat3(transpose(inverse(u_Model))) * normal;

	v_WorldPosition = (u_Model * vec4(position, 1.0)).xyz; //정점의 World space 좌표

	//이 셰이더의 u_View는 카메라 기준의 view 행렬임
	//그림자 계산을 위해서는 해당 fragment가 카메라 기준으로 어느 위치인지 알아야 하기 때문에
	//아래와 같이 계산하여 varying으로 fragment로 넘김
	v_DirectionalLightSpacePos = u_DirectionalLightTransform * u_Model*vec4(position, 1.0);
};

#shader fragment
#version 330

layout(location = 0) out vec4 color;

//셰이더에서도 Composition을 통한 구조 정리
struct Light
{
	vec3 lightColor;
	float ambientIntensity;
	float diffuseIntensity;
};

//셰이더 코드에서 구조체 사용 가능
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

in vec2 v_TexCoord; //버텍스 셰이더에서 넘겨받은 데이터
in vec3 v_Normal;
in vec3 v_WorldPosition;
in vec4 v_DirectionalLightSpacePos; //Light 기준의 fragment position

uniform vec3 u_EyePosition; //specular 계산을 위한 카메라 위치
uniform sampler2D u_Texture; //texture는 sampler2D 타입
uniform DirectionalLight u_DirectionalLight;
uniform Material u_Material;
uniform sampler2D u_DirectionalShadowMap; //shadow map 텍스처

float CalcDirectionalShadowFactor(DirectionalLight light)
{
	//이 정보는 perspective division을 자동으로 하지 않기때문에 직접 해주어야 함
	vec3 projCoords = v_DirectionalLightSpacePos.xyz / v_DirectionalLightSpacePos.w;
	projCoords = (projCoords * 0.5) + 0.5; //[-1,1] (NDC) -> [0,1] (Depth) 변환

	float closest = texture(u_DirectionalShadowMap, projCoords.xy).r; //shadow map 텍스처에 저장된 깊이 정보 샘플링
	float current = projCoords.z; //실제 frag와 light간의 깊이

	//간단한 고정된 bias 대신 계산을 통해 적절한 bias 선정
	vec3 normal = normalize(v_Normal);
	vec3 lightDir = normalize(light.direction);
	float bias = max(0.05*(1.0 - dot(normal, lightDir)),0.055);

	float shadow = current - bias > closest ? 1.0 : 0.0; //bias 적용

	if (projCoords.z > 1.0) //far plane 뒤쪽으로는 shadow 적용 안함
	{
		shadow = 0.0;
	}

	return shadow;
}

//빛 기본 정보와 방향이 주어졌을 때, rendering eqn에 따른 색상 계산
vec3 CalcLight(Light light, vec3 direction, float shadowFactor) //shadow Factor 추가
{
	vec3 lightAmbient = light.lightColor * light.ambientIntensity;
	vec3 normal = normalize(v_Normal); //(주의!)법선 벡터 정규화 해야 함!

	vec3 lightDir = normalize(-direction);
	float diffuseFactor = max(dot(normal, lightDir), 0.0);
	vec3 lightDiffuse = light.lightColor * light.diffuseIntensity * diffuseFactor;

	vec3 fragToEye = normalize(u_EyePosition - v_WorldPosition);
	vec3 rVec = 2.0 * normal * dot(normal, lightDir) - lightDir; //r vector 계산
	vec3 lightSpecular = pow(max(dot(rVec, fragToEye), 0.0), u_Material.shininess) * light.lightColor * u_Material.specularIntensity;

	//shadow factor가 1이면 diffuse와 specular 적용 안함(ambient는 전역 조명이므로 그림자여도 적용)
	return (lightAmbient + (1.0-shadowFactor)*(lightDiffuse + lightSpecular)); //light와 direction에 의한 light color 반환
}

//Directional Light의 경우 색상 계산
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