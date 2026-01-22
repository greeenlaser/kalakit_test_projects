#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec4 aTangent;

out vec3 vNormal;
out vec3 vTangent;
out vec3 vBitangent;
out vec2 vTexCoord;

out vec3 vFragPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
	vec4 worldPos = uModel * vec4(aPos, 1.0);
	vFragPos = worldPos.xyz;
	
	mat3 normalMatrix = mat3(uModel);
	
	vec3 N = normalize(normalMatrix * aNormal);
	vec3 T = normalize(normalMatrix * aTangent.xyz);
	vec3 B = normalize(cross(N, T) * aTangent.w);
	
	vNormal    = N;
	vTangent   = T;
	vBitangent = B;
	
	vTexCoord = aTexCoord;
	
	gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);
}