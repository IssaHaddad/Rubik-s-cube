#version 450 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D screenTexture;

uniform float time;
const float blurSizeH = 1.0 / 500.0;
const float blurSizeV = 1.0 / 500.0;

void main()
{ 

	//color = texture( screenTexture, TexCoords + 0.005*vec2( sin(time+1024.0*TexCoords.x),cos(time+768.0*TexCoords.y)) ).xyz;
	vec4 sum = vec4(0.0);
    for (int x = -4; x <= 4; x++)
        for (int y = -4; y <= 4; y++)
            sum += texture(
                screenTexture,
                vec2(TexCoords.x + x * blurSizeH, TexCoords.y + y * blurSizeV)
            ) / 81.0;
    color = sum;
}
