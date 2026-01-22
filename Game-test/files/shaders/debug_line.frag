#version 330 core

in float vDist;
out vec4 FragColor;

uniform float uOpacity;     //makes the model transparent if below 1.0
uniform vec3 uDiffuseColor; //line color
uniform float uThickness;   //line width in pixels
uniform float uFadeSize;    //smooth fade zone width

void main()
{
	float opacity = clamp(uOpacity, 0.0, 1.0);
	vec3 color = clamp(uDiffuseColor, 0.0, 1.0);

	//skip if too transparent
	if (opacity < 0.001) discard;
	
	float halfWidth = uThickness * 0.5f;
	
	//scale vDist into actual width space
	float dist = abs(vDist) * halfWidth;
	
	//skip if outside the band
	if (dist > halfWidth) discard;
	
	//smooth fade zone
	float edge = halfWidth - uFadeSize;
	
	//1.0 if distance <= edge, fades to 0.0 near halfWidth
	float smoothAlpha = smoothstep(halfWidth, edge, dist);
	
	FragColor = vec4(color, opacity * smoothAlpha);
}