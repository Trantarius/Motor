#version 450 core
out vec4 FragColor;

uniform vec3 color;

void main()
{
    FragColor.rgb = color;
    FragColor.a=1.0;
}
