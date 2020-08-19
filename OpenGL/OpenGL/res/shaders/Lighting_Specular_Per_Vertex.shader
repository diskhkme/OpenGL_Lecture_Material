#shader vertex
#version 330

//Model 클래스를 보면 3개의 좌표, 2개의 텍스처 좌표, 3개의 법선 벡터 데이터를 넘겨주는 것을 알 수 있음
//이런 순서로 넘어오도록 우리가 vertex buffer layout을 통해 정의했음
layout(location = 0) in vec3 position; 
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec2 v_TexCoord; //버텍스 셰이더에서 추가적으로 출력하는 데이터(varying)
out vec3 v_Normal;
out vec3 v_View; //specular 계산을 위한 카메라로 향하는 벡터

uniform mat4 u_Model; //world 변환 행렬
uniform mat4 u_View; //카메라를 통해 변화된 View 행렬
uniform mat4 u_Projection; //CPU에서 전달될 (glm 라이브러리를 통해 생성된) 투영 행렬
uniform vec3 u_EyePosition; //specular 계산을 위한 카메라 위치

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(position,1.0); // 정점 위치를 투영 행렬과 곱
	v_TexCoord = texCoord; //vertex 변환에 따라 바뀌지 않으므로 그대로 넘겨주면 됨

	//https://www.youtube.com/watch?v=d3EUd2HxsO4&app=desktop
	v_Normal = mat3(transpose(inverse(u_Model))) * normal;

	vec3 worldPos = (u_Model * vec4(position, 1.0)).xyz; //정점의 World space 좌표
	v_View = normalize(u_EyePosition - worldPos);
};

#shader fragment
#version 330

layout(location = 0) out vec4 color;

//셰이더 코드에서 구조체 사용 가능
struct DirectionalLight
{
	vec3 lightColor;
	//for ambient
	float ambientIntensity;
	//for diffuse 
	vec3 direction;
	float diffuseIntensity;
};

struct Material
{
	float specularIntensity;
	float shininess; //sh
};

in vec2 v_TexCoord; //버텍스 셰이더에서 넘겨받은 데이터
in vec3 v_Normal;
in vec3 v_View;

uniform sampler2D u_Texture; //texture는 sampler2D 타입
uniform DirectionalLight u_DirectionalLight;
uniform Material u_Material;

void main()
{
	vec3 lightAmbient = u_DirectionalLight.lightColor * u_DirectionalLight.ambientIntensity;

	//병렬 성능을 위해서 if 분기문은 만들지 않는 것이 필요! (대신 max 함수 사용)
	//light 방향을 뒤집는 것에 주의
	//normal은 단위 벡터로 넘어오지만, scan conversion동안 보간되어 길이가 변할 수 있으므로 다시 normalization 필요!
	vec3 lightDir = normalize(-u_DirectionalLight.direction);
	float diffuseFactor = max(dot(normalize(v_Normal), lightDir), 0.0);
	vec3 lightDiffuse = u_DirectionalLight.lightColor * u_DirectionalLight.diffuseIntensity * diffuseFactor;

	vec3 rVec = 2.0 * v_Normal * dot(v_Normal, lightDir) - lightDir; //r vector 계산
	vec3 lightSpecular = pow(max(dot(rVec, v_View), 0.0), u_Material.shininess) * u_DirectionalLight.lightColor * u_Material.specularIntensity;

	color = texture(u_Texture, v_TexCoord) * vec4(lightAmbient + lightDiffuse + lightSpecular,1.0);
	//color = texture(u_Texture, v_TexCoord) * vec4(lightAmbient + lightDiffuse, 1.0);
	//color = vec4(1.0, 0.0, 0.0, 1.0);
};