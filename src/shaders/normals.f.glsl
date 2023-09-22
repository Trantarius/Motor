#version 330 core
out vec4 FragColor;

in vec3 Vertex;
in vec3 Normal;
in vec2 UV;
in vec3 Color;

void main()
{
    FragColor.rgb = (Normal + vec3(1.0))/2.0;
    FragColor.a=1.0;
}
