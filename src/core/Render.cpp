#include "Render.hpp"


void Viewport::render(){
  preRender();

  assert(camera!=nullptr);
  current_projection=camera->getProjection(size);
  current_view=camera->getView();

  // update light buffer

  //Note: size must match max light count in light.glsl
  fvec3 colors[8];
  float lumins[8];
  float spreads[8];
  fmat4 transforms[8];
  int types[8];

  int idx=0;
  for(auto it=lights.begin();it!=lights.end();){
    if(!*it){
      it=lights.erase(it);
      continue;
    }

    std::shared_ptr<Light>& light=*it;
    colors[idx]=light->color;
    lumins[idx]=light->lumin;
    spreads[idx]=light->spread;
    transforms[idx]=current_view*light->transform.toMatrix();
    types[idx]=light->type;

    idx++;
    it++;
  }

  _light_buffer.beginUpdate();
  _light_buffer.setUniform("light_count"_id,idx);
  _light_buffer.setUniformArray("light_colors"_id,Bloc(colors,8));
  _light_buffer.setUniformArray("light_lumins"_id,Bloc(lumins,8));
  _light_buffer.setUniformArray("light_spreads"_id,Bloc(spreads,8));
  _light_buffer.setUniformArray("light_transforms"_id,Bloc(transforms,8));
  _light_buffer.setUniformArray("light_types"_id,Bloc(types,8));
  _light_buffer.endUpdate();


  for(auto it=objects.begin();it!=objects.end();){
    std::shared_ptr<Renderable> strong = it->lock();
    if(!strong){
      it=objects.erase(it);
      continue;
    }
    strong->render(mode);
    it++;
  }

  postRender();
}

Viewport::Viewport(){
  Shader shader("/mesh.v.glsl","/lighttest.f.glsl");
  UniformBlock block=shader.getUniformBlock("LightBlock"_id);
  _light_buffer=UniformBuffer(block);
}

void Viewport::addLight(std::shared_ptr<Light> light){
  lights.push_back(light);
}

void Viewport::removeLight(std::shared_ptr<Light> light){
  lights.remove(light);
}

fmat4 Camera::getView() const {
  return transform.toInvMatrix();
}

fmat4 PerspectiveCamera::getProjection(ivec2 vp_size) const {
  //http://www.songho.ca/opengl/gl_projectionmatrix.html
  float height=tan(fov/2)*near;
  float width=height * (float)vp_size.x/vp_size.y;
  return fmat4(
    near/width,   0,    0,    0,
    0,    near/height,    0,    0,
    0,    0,    (far+near)/(near-far), 2*far*near/(near-far),
    0,    0,    -1,   0
  );
}

fmat4 OrthographicCamera::getProjection(ivec2 vp_size) const {
  float aspect=(float)vp_size.x/vp_size.y;
  float height=size;
  float width=height*aspect;
  return fmat4(
    1/width,    0,    0,    0,
    0,    1/height,   0,    0,
    0,    0,    2/(near-far),   (near+far)/(near-far),
    0,    0,    0,    1
  );
}
