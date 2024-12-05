#version 330 core 

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inColor; //Koordinate texture, propustamo ih u FS kao boje
out vec4 color;

void main()
{
	gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0);
	color = inColor;
}