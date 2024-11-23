#version 450 core
layout(location=0) out vec3 fLight;

in vec2 vUV;

uniform sampler2DArray material_buffer;
uniform int mode_selection=0;

void main()
{
	//fLight = vec3(vUV.xy,0);
	//fLight = texelFetch(material_buffer, ivec3(ivec2(gl_FragCoord.xy),mode_selection), 0).rgb;
	fLight = texture(material_buffer, vec3(vUV,float(mode_selection))).rgb;
}
