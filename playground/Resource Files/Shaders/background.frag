#version 330 core

in vec2 TexCoord;

uniform sampler2D backgroundTexture;

out vec4 FragColor;

void main()
{
    FragColor = texture(backgroundTexture, TexCoord);
}
