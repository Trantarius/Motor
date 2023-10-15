
const int MAX_LIGHT_COUNT=8;

const int DIRECTIONAL=1;
const int POINT=2;
const int LINE=3;
const int PLANE=4;

struct Light{
  int type;

  //light local space to view space
  mat4 transform;

  //parameter that determines energy and luminosity of light
  float lumin;

  //increases width of emmitting area
  //when <1, uses more energy conserving method
  //when >1, uses more luminosity conserving method
  float spread;

  vec3 color;
};

struct Material{
  vec3 albedo;
  float rough;
  float metal;
  float spec;
};

struct Surface{
  vec3 vertex;
  vec3 normal;
  vec3 refl;
  vec3 view;
};

layout(shared) uniform LightBlock{
  //Light lights[MAX_LIGHT_COUNT];
  int light_types[MAX_LIGHT_COUNT];
  mat4 light_transforms[MAX_LIGHT_COUNT];
  float light_lumins[MAX_LIGHT_COUNT];
  float light_spreads[MAX_LIGHT_COUNT];
  vec3 light_colors[MAX_LIGHT_COUNT];
  int light_count;
};

float phongMix(float diffuse,float specular,Material mat){
  return diffuse+pow(specular,1.0/(mat.rough+1))*mat.spec;
}

float attenuate(float dist,Light light){

  float atten=(light.spread + exp(-light.spread))/(dist*dist + light.spread);
//   above is a smoother version of:
//   if(light.spread<1){
//     atten=1.0/(dist*dist+light.spread);
//   }else{
//     atten=light.spread/(dist*dist+light.spread);
//   }

  return light.lumin*atten;
}

float dirLight(Light light, Material mat,Surface surf){
  vec3 ldir = normalize(mat3(light.transform)*vec3(0,0,-1));

  float diffuse = max(0,dot(surf.normal,ldir));
  float specular=max(0,dot(surf.refl,ldir));

  return phongMix(diffuse,specular,mat)*light.lumin;
}

float pointLight(Light light,Material mat,Surface surf){
  vec3 lpos=(light.transform*vec4(0,0,0,1)).xyz;
  vec3 ldir=(lpos-surf.vertex);
  float dist=length(ldir);
  ldir/=dist;

  float diffuse = max(0,dot(surf.normal,ldir));
  float specular=max(0,dot(surf.refl,ldir));

  return phongMix(diffuse,specular,mat)*attenuate(dist,light);
}


vec3 getLightEffect(Light light,Material mat,Surface surf){
  if(light.type==DIRECTIONAL){
    float strength=dirLight(light,mat,surf);
    return strength*light.color*mat.albedo;
  }
  else if(light.type==POINT){
    float strength=pointLight(light,mat,surf);
    return strength*light.color*mat.albedo;
  }
  return vec3(0);
}
