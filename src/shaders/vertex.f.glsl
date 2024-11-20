#version 450 core
out vec4 FragColor;

in vec3 Vertex;

void main()
{
    FragColor.rgb = Vertex;
    FragColor.a=1.0;
}
