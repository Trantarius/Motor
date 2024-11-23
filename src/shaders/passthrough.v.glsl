#version 450 core
layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

out vec3 vVertex;
out vec3 vNormal;
out vec2 vUV;

void main()
{
	vVertex = aVertex;
	vNormal = aNormal;
	vUV = aUV;
	gl_Position.xyz = aVertex;
	gl_Position.w = 1;
}
