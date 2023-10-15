#version 450 core
#extension GL_ARB_shading_language_include : require
#include "/light.glsl"

out vec4 FragColor;

in vec3 Normal;
in vec3 Vertex;

uniform mat4 view;


void main()
{
  Material mat;
  mat.albedo=vec3(1,1,1);
  mat.rough=0.1;
  mat.metal=0;
  mat.spec=1.0;

  Surface surf;
  surf.vertex=Vertex;
  surf.normal=Normal;
  surf.view=normalize(Vertex);
  surf.refl=reflect(surf.view,Normal);

  vec3 col=vec3(0);
  for(int l=0;l<light_count;l++){
    Light light;
    light.type=light_types[l];
    light.transform=light_transforms[l];
    light.lumin=light_lumins[l];
    light.spread=light_spreads[l];
    light.color=light_colors[l];
    col+=getLightEffect(light,mat,surf);
  }

  FragColor.rgb = col;
  FragColor.a=1.0;
}
