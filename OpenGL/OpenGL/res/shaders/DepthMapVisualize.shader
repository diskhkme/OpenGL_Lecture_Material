#shader vertex
#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

uniform mat4 u_Model; //world 변환 행렬
uniform mat4 u_View; //카메라를 통해 변화된 View 행렬
uniform mat4 u_Projection; //CPU에서 전달될 (glm 라이브러리를 통해 생성된) 투영 행렬

out vec2 v_TexCoord;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0);
	v_TexCoord = texCoord;
}

#shader fragment
#version 330

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_DepthMap;

void main()
{
	float depthValue = texture(u_DepthMap, v_TexCoord).r;
	color = vec4(vec3(depthValue), 1.0);
	//color = vec4(1.0, 0.0, 0.0, 1.0);
}