#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D tex1;
uniform sampler2D tex2;

void main()
{
    color = mix(texture(tex1, TexCoords), texture(tex2, TexCoords * 75), 0.3);
}
