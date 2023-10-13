#version 450 core
out vec4 FragColor;

uniform sampler2D tex;

in vec2 UV;

void main()
{
    FragColor=texture(tex,UV);
}
