#version 450 core
out vec4 fColor;

in vec3 vVertex;
in vec3 vNormal;
in vec2 vUV;

uniform sampler2D color_texture;

void main()
{
	fColor = texture(color_texture,vUV);
	
	if(vUV.x<0.05||vUV.x>0.95||vUV.y<0.05||vUV.y>0.95){
		fColor = vec4(vUV,0.5,1);
	}
}
