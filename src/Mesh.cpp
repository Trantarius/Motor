#include "Mesh.hpp"
#include "main.hpp"
#include "util/io.hpp"
#include "util/gl_enum_names.hpp"
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
  checkGLError();
}

bool MeshData::is_null() const {
  return info==nullptr;
}

void Mesh::render(Render* renderer){
  if(!shader.is_null() && !mesh_data.is_null()){
    shader.use();
    shader.setUniform("model",getTransform());
    shader.setUniform("view",renderer->getView());
    shader.setUniform("projection",renderer->getProjection());
    mesh_data.draw(GL_TRIANGLES);
  }
}






Array<string> splitString(string str, string delim){
  Array<string> ret;
  size_t start=0;
  for(size_t n=0;n<str.size();n++){
    if(delim.find(str[n])!=string::npos){
      if(n!=start){
        ret.push_back(str.substr(start,n-start));
      }
      n++;
      start=n;
    }
  }

  if(start<str.size()){
    ret.push_back(str.substr(start));
  }
  return ret;
}

int countChar(string str,char c){
  int count=0;
  for(int n=0;n<str.size();n++){
    if(str[n]==c){
      count++;
    }
  }
  return count;
}

MeshData MeshData::readOBJ(string path){
  Bloc<char> filebloc = readfile(path);
  string file(filebloc.ptr,filebloc.size);
  filebloc.destroy();

  Array<vec3> verts;
  Array<vec2> uvs;
  Array<vec3> norms;
  Array<vec3> colors;
  Array<Array<ivec3>> faces;

  Array<string> lines=splitString(file,"\n\r");

  for(string& line : lines){
    Array<string> words=splitString(line," ");

    if(words.empty()){
      continue;
    }

    if(words[0]=="v"){
      if(words.size()<4){
        continue;
      }
      verts.push_back(vec3(stod(words[1]),stod(words[2]),stod(words[3])));

      if(words.size()<7){
        continue;
      }
      colors.push_back(vec3(stod(words[4]),stod(words[5]),stod(words[6])));
    }

    else if(words[0]=="vn"){
      if(words.size()<4){
        continue;
      }
      norms.push_back(vec3(stod(words[1]),stod(words[2]),stod(words[3])));
    }

    else if(words[0]=="vt"){
      if(words.size()<3){
        continue;
      }
      uvs.push_back(vec2(stod(words[1]),stod(words[2])));
    }

    else if(words[0]=="f"){
      if(words.size()<4){
        continue;
      }
      if(words.size()>4){
        printerr("Non-triangular face in ",path);
        continue;
      }

      Array<ivec3> face;
      for(int c=1;c<words.size();c++){
        int slash_count=countChar(words[c],'/');
        ivec3 corner(-1,-1,-1);

        if(slash_count==0){
          corner.x=stol(words[c])-1;
        }
        else if(slash_count==1){
          Array<string> pts=splitString(words[c],"/");
          if(pts.size()!=2){
            printerr("Bad format (1) found in ",path);
            return MeshData();
          }
          corner.x=stol(pts[0])-1;
          corner.y=stol(pts[1])-1;
        }
        else if(slash_count==2){
          Array<string> pts=splitString(words[c],"/");
          if(pts.size()<2){
            printerr("Bad format (2) found in ",path);
            return MeshData();
          }
          if(pts.size()==2){
            corner.x=stol(pts[0])-1;
            corner.z=stol(pts[1])-1;
          }
          else if(pts.size()==3){
            corner.x=stol(pts[0])-1;
            corner.y=stol(pts[1])-1;
            corner.z=stol(pts[2])-1;
          }
          else{
            printerr("Bad format (3) found in ",path);
            return MeshData();
          }
        }
        else{
          printerr("Bad format (4) found in ",path);
          return MeshData();
        }

        face.push_back(corner);
      }
      faces.push_back(face);
    }
  }
  typedef bool (*compType)(ivec3,ivec3);
  std::map<ivec3,size_t,compType> cornermap(compare);
  Array<uvec3> elements;

  for(Array<ivec3>& face : faces){
    uvec3 element;
    if(face.size()!=3){
      printerr("Non triangular face found in ",path);
      continue;
    }
    for(int c=0;c<face.size();c++){
      if(cornermap.contains(face[c])){
        element[c]=cornermap[face[c]];
      }
      else{
        size_t c_idx=cornermap.size();
        cornermap.emplace(face[c],c_idx);
        element[c]=c_idx;
      }
    }
    elements.push_back(element);
  }

  bool has_color;
  bool has_uv=!uvs.empty();
  bool has_normal=!norms.empty();

  if(colors.empty()){
    has_color=false;
  }
  else if(colors.size()==verts.size()){
    has_color=true;
  }
  else{
    printerr("Bad format (5) found in ",path);
    return MeshData();
  }

  Array<uint> attribute_widths;
  uint total_width=3;
  attribute_widths.push_back(3);
  if(has_color){
    total_width+=3;
    attribute_widths.push_back(3);
  }
  if(has_uv){
    total_width+=2;
    attribute_widths.push_back(2);
  }
  if(has_normal){
    total_width+=3;
    attribute_widths.push_back(3);
  }

  Array<float> vdata(cornermap.size()*total_width);
  for(std::pair<uvec3,size_t> pr : cornermap){
    size_t idx=pr.second*total_width;

    vec3 vert=verts[pr.first.x];
    vdata[idx++]=vert.x;
    vdata[idx++]=vert.y;
    vdata[idx++]=vert.z;

    if(has_color){
      vec3 color=colors[pr.first.x];
      vdata[idx++]=color.r;
      vdata[idx++]=color.g;
      vdata[idx++]=color.b;
    }

    if(has_uv){
      if(pr.first.y<0||pr.first.y>=uvs.size()){
        printerr("Bad format (6) found in ",path);
        return MeshData();
      }
      vec2 uv=uvs[pr.first.y];
      vdata[idx++]=uv.x;
      vdata[idx++]=uv.y;
    }

    if(has_normal){
      if(pr.first.z<0||pr.first.z>=norms.size()){
        printerr("Bad format (7) found in ",path);
        return MeshData();
      }
      vec3 norm=norms[pr.first.z];
      vdata[idx++]=norm.x;
      vdata[idx++]=norm.y;
      vdata[idx++]=norm.z;
    }
  }

  Bloc<float> vdata_bloc(vdata.data(),vdata.size());
  Bloc<uint> attribute_widths_bloc(attribute_widths.data(),attribute_widths.size());
  Bloc<uint> elements_bloc((uint*)elements.data(),elements.size()*3);

  return MeshData(vdata_bloc,attribute_widths_bloc,elements_bloc);
}
