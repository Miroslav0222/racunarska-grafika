#version 330 core

in vec4 color; //koordinate teksture
out vec4 outCol;
uniform float brightness;
void main()
{
	outCol = vec4(color.rgb * brightness,color.a);
}