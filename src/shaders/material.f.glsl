
#version 450 core
layout(location=0) out vec3 fAlbedo;
layout(location=1) out vec3 fNormal;
layout(location=2) out vec3 fVertex;
layout(location=3) out vec3 fRoughDiffuseSpecular;

in vec3 vVertex;
in vec3 vNormal;
in vec2 vUV;

uniform bool enable_albedo_texture = false;
uniform vec3 albedo = vec3(0.5,0.5,0.5);
uniform sampler2D albedo_texture;

uniform bool enable_normal_texture = false;
uniform sampler2D normal_texture;

uniform bool enable_roughness_texture = false;
uniform bool enable_diffuse_texture = false;
uniform bool enable_specular_texture = false;
uniform sampler2D rds_texture;
uniform float roughness = 1.0;
uniform float diffuse = 0.5;
uniform float specular = 0.5;

void main()
{
	if(enable_albedo_texture){
		fAlbedo = texture(albedo_texture, vUV).rgb;
	}else{
		fAlbedo = albedo;
	}

	if(enable_normal_texture){
		//https://learnopengl.com/Advanced-Lighting/Normal-Mapping
		// TODO: tangents aren't implemented yet, so no normal mapping
		fNormal = vNormal;
	}else{
		fNormal = vNormal;
	}

	fVertex = vVertex;

	vec3 rds = vec3(roughness, diffuse, specular);
	if(enable_roughness_texture || enable_diffuse_texture || enable_specular_texture){
		vec3 tex_rds = texture(rds_texture, vUV).rgb;
		if(enable_roughness_texture){
			rds.x = tex_rds.x;
		}
		if(enable_diffuse_texture){
			rds.y = tex_rds.y;
		}
		if(enable_specular_texture){
			rds.z = tex_rds.z;
		}
	}
	fRoughDiffuseSpecular = rds;
}
