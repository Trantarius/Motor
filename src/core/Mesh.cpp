#include "Mesh.hpp"
#include "util/io.hpp"
#include "defs/gl_defs.hpp"
#include "Window.hpp"
#include <map>

MeshData::MeshData(Bloc<fvec3> verts):MeshData(_ref_count_init()){
  assert(verts.size%3==0);
  glGenVertexArrays(1,&data->VAO);
  glGenBuffers(1,&data->VBO);

  glBindVertexArray(data->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, data->VBO);
  glBufferData(GL_ARRAY_BUFFER,verts.size*sizeof(fvec3),verts,GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  data->elemcount=verts.size;
}

MeshData::MeshData(Bloc<fvec3> verts,Bloc<ivec3> faces):MeshData(_ref_count_init()){
  glGenVertexArrays(1,&data->VAO);
  glGenBuffers(1,&data->VBO);
  glGenBuffers(1,&data->EBO);

  glBindVertexArray(data->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, data->VBO);
  glBufferData(GL_ARRAY_BUFFER,verts.size*sizeof(fvec3),verts,GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size*sizeof(ivec3),faces,GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  data->elemcount=faces.size*3;
}
MeshData::MeshData(Bloc<float> vdata,Bloc<uint> attribute_widths):MeshData(_ref_count_init()){
  int stride=0;
  for(int n=0;n<attribute_widths.size;n++){
    stride+=attribute_widths[n];
  }

  assert(vdata.size%stride==0);
  int vertcount=vdata.size/stride;
  assert(vertcount%3==0);

  glGenVertexArrays(1,&data->VAO);
  glGenBuffers(1,&data->VBO);

  glBindVertexArray(data->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, data->VBO);
  glBufferData(GL_ARRAY_BUFFER,vdata.size*sizeof(float),vdata,GL_STATIC_DRAW);

  int off=0;
  for(int a=0;a<attribute_widths.size;a++){
    assert(attribute_widths[a]>0 && attribute_widths[a]<5);
    glVertexAttribPointer(a,attribute_widths[a],GL_FLOAT,GL_FALSE,stride*sizeof(float),(void*)(off*sizeof(float)));
    glEnableVertexAttribArray(a);
    off+=attribute_widths[a];
  }

  glBindVertexArray(0);
  data->elemcount=vertcount;
}
MeshData::MeshData(Bloc<float> vdata,Bloc<uint> attribute_widths,Bloc<uint> elements):MeshData(_ref_count_init()){

  int stride=0;
  for(int n=0;n<attribute_widths.size;n++){
    stride+=attribute_widths[n];
  }
  assert(vdata.size%stride==0);

  glGenVertexArrays(1,&data->VAO);
  glGenBuffers(1,&data->VBO);
  glGenBuffers(1,&data->EBO);

  glBindVertexArray(data->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, data->VBO);
  glBufferData(GL_ARRAY_BUFFER,vdata.size*sizeof(float),vdata,GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size*sizeof(uint),elements,GL_STATIC_DRAW);

  int off=0;
  for(int a=0;a<attribute_widths.size;a++){
    assert(attribute_widths[a]>0 && attribute_widths[a]<5);
    glVertexAttribPointer(a,attribute_widths[a],GL_FLOAT,GL_FALSE,stride*sizeof(float),(void*)(off*sizeof(float)));
    glEnableVertexAttribArray(a);
    off+=attribute_widths[a];
  }

  glBindVertexArray(0);
  data->elemcount=elements.size;
}


void MeshData::onDestroy(){
  glDeleteBuffers(1,&data->VBO);
  glDeleteBuffers(1,&data->EBO);
  glDeleteVertexArrays(1,&data->VAO);
}

void MeshData::draw(uint mode) const {
  assert(data!=nullptr);
  glBindVertexArray(data->VAO);
  if(data->EBO==0){
    glDrawArrays(mode,0,data->elemcount);
  }else{
    glDrawElements(mode,data->elemcount,GL_UNSIGNED_INT,0);
  }
  glBindVertexArray(0);
  checkGLError();
}

void Mesh::render(int mode) {
  if(!shader.isNull() && !mesh_data.isNull()){
    shader.use();
    shader.setUniform("model"_id,(fmat4)transform.toMatrix());
    shader.setUniform("view"_id,(fmat4)Window::viewport().view);
    shader.setUniform("projection"_id,(fmat4)Window::viewport().projection);

    if(shader.hasUniformBlock("LightBlock"_id)){
      shader.setUniformBlock("LightBlock"_id,Window::viewport().light_buffer);
    }

    mesh_data.draw(GL_TRIANGLES);
  }
}






std::vector<string> splitString(string str, string delim){
  std::vector<string> ret;
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

  std::vector<fvec3> verts;
  std::vector<fvec2> uvs;
  std::vector<fvec3> norms;
  std::vector<fvec3> colors;
  std::vector<std::vector<ivec3>> faces;

  std::vector<string> lines=splitString(file,"\n\r");

  for(string& line : lines){
    std::vector<string> words=splitString(line," ");

    if(words.empty()){
      continue;
    }

    if(words[0]=="v"){
      if(words.size()<4){
        continue;
      }
      verts.push_back(fvec3(stod(words[1]),stod(words[2]),stod(words[3])));

      if(words.size()<7){
        continue;
      }
      colors.push_back(fvec3(stod(words[4]),stod(words[5]),stod(words[6])));
    }

    else if(words[0]=="vn"){
      if(words.size()<4){
        continue;
      }
      norms.push_back(fvec3(stod(words[1]),stod(words[2]),stod(words[3])));
    }

    else if(words[0]=="vt"){
      if(words.size()<3){
        continue;
      }
      uvs.push_back(fvec2(stod(words[1]),stod(words[2])));
    }

    else if(words[0]=="f"){
      if(words.size()<4){
        continue;
      }
      if(words.size()>4){
        throw IOError("Non-triangular face",path);
      }

      std::vector<ivec3> face;
      for(int c=1;c<words.size();c++){
        int slash_count=countChar(words[c],'/');
        ivec3 corner(-1,-1,-1);

        if(slash_count==0){
          corner.x=stol(words[c])-1;
        }
        else if(slash_count==1){
          std::vector<string> pts=splitString(words[c],"/");
          if(pts.size()!=2){
            throw IOError("Bad format (OBJ)",path);
          }
          corner.x=stol(pts[0])-1;
          corner.y=stol(pts[1])-1;
        }
        else if(slash_count==2){
          std::vector<string> pts=splitString(words[c],"/");
          if(pts.size()<2){
            throw IOError("Bad format (OBJ)",path);
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
            throw IOError("Bad format (OBJ)",path);
          }
        }
        else{
          throw IOError("Bad format (OBJ)",path);
        }

        face.push_back(corner);
      }
      faces.push_back(face);
    }
  }
  std::map<ivec3,size_t> cornermap;
  std::vector<ivec3> elements;

  for(std::vector<ivec3>& face : faces){
    ivec3 element;
    if(face.size()!=3){
      throw IOError("Non triangular face",path);
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
    throw IOError("Bad format (OBJ)",path);
  }

  std::vector<uint> attribute_widths;
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

  std::vector<float> vdata(cornermap.size()*total_width);
  for(std::pair<ivec3,size_t> pr : cornermap){
    size_t idx=pr.second*total_width;

    fvec3 vert=verts[pr.first.x];
    vdata[idx++]=vert.x;
    vdata[idx++]=vert.y;
    vdata[idx++]=vert.z;

    if(has_color){
      fvec3 color=colors[pr.first.x];
      vdata[idx++]=color.x;
      vdata[idx++]=color.y;
      vdata[idx++]=color.z;
    }

    if(has_uv){
      if(pr.first.y<0||pr.first.y>=uvs.size()){
        throw IOError("Bad format (OBJ)",path);
      }
      fvec2 uv=uvs[pr.first.y];
      vdata[idx++]=uv.x;
      vdata[idx++]=uv.y;
    }

    if(has_normal){
      if(pr.first.z<0||pr.first.z>=norms.size()){
        throw IOError("Bad format (OBJ)",path);
      }
      fvec3 norm=norms[pr.first.z];
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
