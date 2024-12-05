#version 330 core

in vec2 chTex; //koordinate teksture
out vec4 outCol;
uniform float brightness;

uniform sampler2D uTex; //teksturna jedinica

void main()
{
	vec4 color = texture(uTex, chTex);
	if(color.b < 0.1 && color.r <0.1 && color.g < 0.1)
		discard;
	else{
		color.r = 1.0;
		color.g = 1.0;
		color.b = 1.0;
	}
	outCol = vec4(color.rgb*brightness, color.a);
}