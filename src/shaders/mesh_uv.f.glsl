#version 450 core
out vec4 fColor;

in vec2 vUV;

void main()
{
	fColor.rg = vUV;
	fColor.b = 0.0;
	fColor.a = 1.0;
}
