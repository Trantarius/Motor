#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNorm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Vertex;
out vec3 Normal;
out vec2 UV;

void main()
{

    vec3 viewPos = (view * model * vec4(aPos*0.5, 1.0)).xyz;
    Vertex = viewPos;
    Normal = normalize(mat3(view) * mat3(model) * aNorm);
    UV = aUV;
    //Color = aColor;
    gl_Position = projection * vec4(viewPos,1);
}
