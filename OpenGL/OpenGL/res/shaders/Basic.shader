#shader vertex
#version 330

layout(location = 0) in vec4 position; 
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord; //버텍스 셰이더에서 추가적으로 출력하는 데이터(varying)

void main()
{
	gl_Position = position; 
	v_TexCoord = texCoord; //vertex 변환에 따라 바뀌지 않으므로 그대로 넘겨주면 됨
};

#shader fragment
#version 330

layout(location = 0) out vec4 color;

in vec2 v_TexCoord; //버텍스 셰이더에서 넘겨받은 데이터

uniform vec4 u_Color; //uniform에는 id가 할당됨
uniform sampler2D u_Texture; //texture는 sampler2D 타입

void main()
{
	vec4 texColor = texture(u_Texture, v_TexCoord); //texture 함수는 texcoord위치의 픽셀 색상 값을 정의된 방법에 따라 샘플링
	color = texColor; //이제 텍스처 이미지에서 샘플링한 색상을 fragment 색상으로 결정
};