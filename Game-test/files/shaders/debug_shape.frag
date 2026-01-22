#version 330 core

out vec4 FragColor;

uniform float uOpacity;     //makes the model transparent if below 1.0
uniform vec3 uDiffuseColor; //base color of the model

void main()
{
	float opacity = clamp(uOpacity, 0.0, 1.0);
	vec3 color = clamp(uDiffuseColor, 0.0, 1.0);

	if (opacity < 0.001) discard;
	
	FragColor = vec4(color, opacity);
}