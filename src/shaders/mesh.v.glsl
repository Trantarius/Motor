#version 450 core
layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

out vec3 vVertex;
out vec3 vNormal;
out vec2 vUV;

void main()
{
	vVertex = (view_matrix * model_matrix * vec4(aVertex, 1.0)).xyz;
	vNormal = normalize(mat3(view_matrix) * mat3(model_matrix) * aNormal);
	vUV = aUV;
	gl_Position = projection_matrix * vec4(vVertex,1);
}
