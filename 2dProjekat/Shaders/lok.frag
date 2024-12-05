#version 330 core

in vec2 chTex; //koordinate teksture
out vec4 outCol;

uniform sampler2D uTex; //teksturna jedinica
uniform float visibility;
uniform float pulseStrength;
uniform float alphaStrength;

void main()
{
	vec4 color = texture(uTex, chTex);
	if(color.r < 0.1)
		discard;

	if(color.g > 0.3){
		color.g = 0.2;
	}
	if(color.b > 0.3){
		color.b = 0.2;
	}
	if(color.r> 0.3){
		color.r = 0.2;
	}
	color.b += pulseStrength;
	
	outCol = vec4(color.r, color.g, color.b, color.a*visibility - alphaStrength);
}