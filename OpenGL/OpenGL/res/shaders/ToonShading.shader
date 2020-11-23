#shader vertex
#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

uniform mat4 u_Model;
uniform mat4 u_View; 
uniform mat4 u_Projection;

out vec2 v_TexCoord;
out vec3 v_Normal;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0);
	v_Normal = mat3(transpose(inverse(u_Model))) * normal;
	v_TexCoord = texCoord;
}

#shader fragment
#version 330

struct Light
{
	vec3 lightColor;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight
{
	Light base;
	vec3 direction;
};

layout(location = 0) out vec4 color;

uniform DirectionalLight u_DirectionalLight;
//uniform sampler2D u_Texture;
uniform sampler2D u_ToonLut; //Look up table
in vec2 v_TexCoord;
in vec3 v_Normal;

void main()
{
	vec3 normal = normalize(v_Normal);

	vec3 lightDir = normalize(-u_DirectionalLight.direction);
	float ndotl = dot(normal, lightDir);
	float diffuseFactor = max(ndotl, 0.0);
	
	vec4 lut = texture(u_ToonLut, vec2(diffuseFactor, 0));

	color = vec4(0.8, 0.3, 0.2, 1.0) * lut;
	/*color = texture(u_Texture, v_TexCoord) * lut;
	color = lut;*/
}