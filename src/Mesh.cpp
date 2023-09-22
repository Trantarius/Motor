#include "Mesh.hpp"
#include "main.hpp"




MeshData::MeshData(Bloc<vec3> verts){
  assert(verts.size%3==0);
  info=new Info();
  info->refcount=1;
  glGenVertexArrays(1,&info->VAO);
  glGenBuffers(1,&info->VBO);

  glBindVertexArray(info->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, info->VBO);
  glBufferData(GL_ARRAY_BUFFER,verts.size*sizeof(vec3),verts,GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  info->elemcount=verts.size;
}

MeshData::MeshData(Bloc<vec3> verts,Bloc<ivec3> faces){
  info=new Info();
  info->refcount=1;
  glGenVertexArrays(1,&info->VAO);
  glGenBuffers(1,&info->VBO);
  glGenBuffers(1,&info->EBO);

  glBindVertexArray(info->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, info->VBO);
  glBufferData(GL_ARRAY_BUFFER,verts.size*sizeof(vec3),verts,GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, info->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size*sizeof(ivec3),faces,GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  info->elemcount=faces.size*3;
}
MeshData::MeshData(Bloc<float> vdata,Bloc<uint> attribute_widths){
  int stride=0;
  for(int n=0;n<attribute_widths.size;n++){
    stride+=attribute_widths[n];
  }

  assert(vdata.size%stride==0);
  int vertcount=vdata.size/stride;
  assert(vertcount%3==0);

  info=new Info();
  info->refcount=1;
  glGenVertexArrays(1,&info->VAO);
  glGenBuffers(1,&info->VBO);

  glBindVertexArray(info->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, info->VBO);
  glBufferData(GL_ARRAY_BUFFER,vdata.size*sizeof(float),vdata,GL_STATIC_DRAW);

  int off=0;
  for(int a=0;a<attribute_widths.size;a++){
    assert(attribute_widths[a]>0 && attribute_widths[a]<5);
    glVertexAttribPointer(a,attribute_widths[a],GL_FLOAT,GL_FALSE,stride*sizeof(float),(void*)(off*sizeof(float)));
    glEnableVertexAttribArray(a);
    off+=attribute_widths[a];
  }

  glBindVertexArray(0);
  info->elemcount=vertcount;
}
MeshData::MeshData(Bloc<float> vdata,Bloc<uint> attribute_widths,Bloc<uint> elements){

  int stride=0;
  for(int n=0;n<attribute_widths.size;n++){
    stride+=attribute_widths[n];
  }
  assert(vdata.size%stride==0);

  info=new Info();
  info->refcount=1;
  glGenVertexArrays(1,&info->VAO);
  glGenBuffers(1,&info->VBO);
  glGenBuffers(1,&info->EBO);

  glBindVertexArray(info->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, info->VBO);
  glBufferData(GL_ARRAY_BUFFER,vdata.size*sizeof(float),vdata,GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, info->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size*sizeof(uint),elements,GL_STATIC_DRAW);

  int off=0;
  for(int a=0;a<attribute_widths.size;a++){
    assert(attribute_widths[a]>0 && attribute_widths[a]<5);
    glVertexAttribPointer(a,attribute_widths[a],GL_FLOAT,GL_FALSE,stride*sizeof(float),(void*)(off*sizeof(float)));
    glEnableVertexAttribArray(a);
    off+=attribute_widths[a];
  }

  glBindVertexArray(0);
  info->elemcount=elements.size;
}


void MeshData::dispose(){
  glDeleteVertexArrays(1,&info->VAO);
  glDeleteBuffers(1,&info->VBO);
  glDeleteBuffers(1,&info->EBO);
  delete info;
  info=nullptr;
}

MeshData::~MeshData(){
  info->refcount--;
  if(info->refcount==0){
    dispose();
  }
}

void MeshData::operator=(const MeshData& md){
  if(info!=nullptr){
    info->refcount--;
    if(info->refcount==0){
      dispose();
    }
  }

  info=md.info;
  if(info!=nullptr){
    info->refcount++;
  }
}

void MeshData::draw(uint mode) const {
  assert(info!=nullptr);
  glBindVertexArray(info->VAO);
  if(info->EBO==0){
    glDrawArrays(mode,0,info->elemcount);
  }else{
    glDrawElements(mode,info->elemcount,GL_UNSIGNED_INT,0);
  }
  glBindVertexArray(0);
}

bool MeshData::is_null() const {
  return info==nullptr;
}

void Mesh::render(){
  if(!shader.is_null() && !mesh_data.is_null()){
    shader.use();
    mesh_data.draw(GL_TRIANGLES);
  }
}
