#version 450 core
out vec3 fLight;

in vec2 vUV;

uniform sampler2DArray material_buffer;
uniform vec3 light_position;
uniform vec3 light_color;
uniform float light_radius;
uniform float light_emission_radius;
uniform float light_strength;

void main()
{
	vec3 albedo = texture(material_buffer, vec3(vUV, 0)).rgb;
	vec3 normal = texture(material_buffer, vec3(vUV, 1)).rgb;
	vec3 vertex = texture(material_buffer, vec3(vUV, 2)).rgb;
	vec3 rds = texture(material_buffer, vec3(vUV, 3)).rgb;
	float roughness = rds.x;
	float diffuse = rds.y;
	float specular = rds.z;

	float frag_distance = length(light_position-vertex);
	float attenuation = (light_strength+light_emission_radius)/((frag_distance/light_radius)*(frag_distance/light_radius) + light_emission_radius);
	vec3 light_dir = (vertex-light_position)/frag_distance;
	float diff = diffuse*max(0, dot(light_dir, -normal));
	float light_refl = reflect(light_dir, normal);
	float spec = specular*max(0, dot(-normalize(view), light_refl));
	float total_strength = light_strength*(diff+spec)*attenuation;

	fLight = light_color*albedo*total_strength;
}
