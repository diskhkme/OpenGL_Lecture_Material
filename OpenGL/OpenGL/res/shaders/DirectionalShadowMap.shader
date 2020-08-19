#shader vertex
#version 330

layout(location = 0) in vec3 position;

uniform mat4 u_Model;
uniform mat4 u_DirectionalLightTransform; //카메라가 directional light 위치와 방향에 있다고 가정한 VP 행렬

void main()
{
	gl_Position = u_DirectionalLightTransform * u_Model * vec4(position, 1.0);
	//gl_Position = vec4(0.0, 0.0, 1000.0, 1.0);
}

#shader fragment
#version 330

//프래그먼트 셰이더는 아무것도 하지 않음. depth 값만 사용할 것이기 때문에

void main()
{
}