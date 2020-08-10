#shader vertex
#version 330 core

layout(location = 0) in vec4 position; 

void main()
{
	gl_Position = position; 
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color; //uniform에는 id가 할당됨

void main()
{
	color = u_Color; //이제 CPU에서 전달된 u_Color를 출력 색상으로 사용
};