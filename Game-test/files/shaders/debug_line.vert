#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float aDist;

out float vDist;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform float uThickness; //pixel width

void main()
{
	vDist = aDist;
	
	gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);
}