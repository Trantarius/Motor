#include "Mesh.hpp"
#include "main.hpp"
#include "util/io.hpp"
#include <fstream>
#include <map>


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
    shader.setUniform("model",(mat4)this->transform);
    shader.setUniform("view",(mat4)inverse(renderer->camera.transform));
    shader.setUniform("projection",(mat4)renderer->camera.projection);
    mesh_data.draw(GL_TRIANGLES);
  }
}






Array<string> splitString(string str,char delim){
  Array<string> ret;
  string acc;
  for(char c : str){
    if(c==delim){
      if(acc!=""){
        ret.push_back(acc);
      }
      acc="";
    }else{
      acc.push_back(c);
    }
  }
  if(acc!=""){
    ret.push_back(acc);
  }
  return ret;
}

MeshData MeshData::readOBJ(string path){
  std::ifstream file(path);

  Array<vec3> verts;
  Array<vec2> uvs;
  Array<vec3> norms;
  Array<vec3> colors;
  Array<Array<string>> faces;

  string line;
  while(true){
    line="";
    getline(file,line);
    if(line==""){
      break;
    }

    Array<string> words=splitString(line,' ');

    if(words[0]=="v"){
      if(words.size()>=4){
        verts.push_back(vec3(stof(words[1]),stof(words[2]),stof(words[3])));
        if(words.size()==7){
          colors.push_back(vec3(stof(words[4]),stof(words[5]),stof(words[6])));
        }
      }
    }
    else if(words[0]=="vt"){
      if(words.size()>=3){
        uvs.push_back(vec2(stof(words[1]),stof(words[2])));
      }
    }
    else if(words[0]=="vn"){
      if(words.size()>=4){
        norms.push_back(vec3(stof(words[1]),stof(words[2]),stof(words[3])));
      }
    }
    else if(words[0]=="f"){
      Array<string> fv;
      for(int w=1;w<words.size();w++){
        fv.push_back(words[w]);
      }
      faces.push_back(fv);
    }
  }

  Map<string,int> vertmap;
  for(Array<string>& arr : faces){
    for(string iv : arr){
      vertmap[iv]=-1;
    }
  }

  Array<float> vertbuffer;
  int idx=0;
  for(std::pair<string,int> pr : vertmap){
    vertmap[pr.first]=idx++;

    string word=pr.first;
    int sep1=word.find_first_of('/');
    int sep2=word.find_last_of('/');

    int vidx=stoi(word.substr(0,sep1));
    vertbuffer.push_back(verts[vidx].x);
    vertbuffer.push_back(verts[vidx].y);
    vertbuffer.push_back(verts[vidx].z);

    if(!uvs.empty()){
      int uvidx=stoi(word.substr(sep1+1,sep2-sep1-1));
      vertbuffer.push_back(uvs[uvidx].x);
      vertbuffer.push_back(uvs[uvidx].y);
    }

    if(!norms.empty()){
      int normidx=stoi(word.substr(sep2+1));
      vertbuffer.push_back(norms[normidx].x);
      vertbuffer.push_back(norms[normidx].y);
      vertbuffer.push_back(norms[normidx].z);
    }

    if(!colors.empty()){
      vertbuffer.push_back(colors[vidx].x);
      vertbuffer.push_back(colors[vidx].y);
      vertbuffer.push_back(colors[vidx].z);
    }
  }

  Array<uint> elembuffer;
  for(Array<string>& face : faces){
    for(string vid : face){
      elembuffer.push_back(vertmap[vid]);
    }
  }

  Array<uint> attribs;
  attribs.push_back(3);
  if(!uvs.empty()){
    attribs.push_back(2);
  }
  if(!norms.empty()){
    attribs.push_back(3);
  }
  if(!colors.empty()){
    attribs.push_back(3);
  }

  MeshData md=MeshData(Bloc<float>(
    vertbuffer.data(),vertbuffer.size()),
    Bloc<uint>(attribs.data(),attribs.size()),
    Bloc<uint>(elembuffer.data(),elembuffer.size())
  );

  return md;
}
