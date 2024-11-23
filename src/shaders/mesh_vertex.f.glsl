#version 450 core
out vec4 fColor;

in vec3 vVertex;

void main()
{
	fColor.rgb = vVertex;
	fColor.a = 1.0;
}
