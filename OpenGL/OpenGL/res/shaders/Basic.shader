#shader vertex
#version 330

//Model 클래스를 보면 3개의 좌표, 2개의 텍스처 좌표, 3개의 법선 벡터 데이터를 넘겨주는 것을 알 수 있음
//이런 순서로 넘어오도록 우리가 vertex buffer layout을 통해 정의했음
layout(location = 0) in vec3 position; 
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec2 v_TexCoord; //버텍스 셰이더에서 추가적으로 출력하는 데이터(varying)
out vec3 v_Normal;

uniform mat4 u_Model; //world 변환 행렬
uniform mat4 u_View; //카메라를 통해 변화된 View 행렬
uniform mat4 u_Projection; //CPU에서 전달될 (glm 라이브러리를 통해 생성된) 투영 행렬

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(position,1.0); // 정점 위치를 투영 행렬과 곱
	v_TexCoord = texCoord; //vertex 변환에 따라 바뀌지 않으므로 그대로 넘겨주면 됨
	v_Normal = normal;
};

#shader fragment
#version 330

layout(location = 0) out vec4 color;

//셰이더 코드에서 구조체 사용 가능
struct DirectionalLight
{
	//강의 자료에서 s_a에 해당 (acolor * aintensity = s_a)
	vec3 lightColor; 
	float ambientIntensity;
};

//-- 아래와 같이 상세하게 Material의 속성별 color를 정하는 경우도 있지만,
//-- 물체의 diffuse color와 ambient/specular 컬러가 크게 다르지 않다고 가정하여 간략화 가능
//-- 즉, m_a = m_d & m_s = m_d 로 가정
//-- m_d는? texture color

//struct Material {
//	vec3 ambient; //m_a
//	vec3 diffuse; //m_d
//	vec3 specular; //m_s
//	float shininess; //sh
//};
//uniform Material u_Material;

in vec2 v_TexCoord; //버텍스 셰이더에서 넘겨받은 데이터
in vec3 v_Normal;

uniform sampler2D u_Texture; //texture는 sampler2D 타입
uniform DirectionalLight u_DirectionalLight;

void main()
{
	vec3 lightAmbient = u_DirectionalLight.lightColor * u_DirectionalLight.ambientIntensity;
	color = texture(u_Texture, v_TexCoord) * vec4(lightAmbient,1.0);
	//color = vec4(1.0, 0.0, 0.0, 1.0);
};