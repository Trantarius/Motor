#version 450 core
out vec4 fColor;

in vec3 vNormal;

void main()
{
	fColor.rgb = vNormal;
	fColor.a = 1.0;
}
