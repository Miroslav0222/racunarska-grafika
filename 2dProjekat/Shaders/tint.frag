#version 330 core

in vec4 color; //koordinate teksture
out vec4 outCol;
uniform float alphaStrength;
void main()
{
	outCol = vec4(color.rgb,color.a * alphaStrength);
}