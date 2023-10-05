#pragma once
#include "Shader.hpp"
#include "Render.hpp"

class MeshData{
  struct Info{
    uint refcount=0;
    uint VAO=0,VBO=0,EBO=0;
    uint elemcount=0;
  };
  Info* info=nullptr;

  void dispose();

public:

  MeshData(Bloc<vec3> verts);
  MeshData(Bloc<vec3> verts,Bloc<ivec3> faces);
  MeshData(Bloc<float> vdata,Bloc<uint> attribute_widths);
  MeshData(Bloc<float> vdata,Bloc<uint> attribute_widths,Bloc<uint> elements);

  MeshData(){}
  MeshData(const MeshData& md){*this=md;}
  ~MeshData();

  void operator=(const MeshData& md);

  void draw(uint mode) const;
  bool is_null() const;

  static MeshData readOBJ(string path);
};

class Mesh : public Renderable, public Spatial{
  void render() override;
public:
  MeshData mesh_data;
  Shader shader;

  Mesh(){}
  Mesh(const MeshData& md):mesh_data(md){}
};
