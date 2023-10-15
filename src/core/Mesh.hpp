#pragma once
#include "Shader.hpp"
#include "Render.hpp"
#include "util/refcount.hpp"
#include "util/Bloc.hpp"

class MeshData{
  struct _Data{
    uint refcount=0;
    uint VAO=0,VBO=0,EBO=0;
    uint elemcount=0;
  };

  REF_COUNTER(MeshData,_Data)

public:

  MeshData(Bloc<fvec3> verts);
  MeshData(Bloc<fvec3> verts,Bloc<ivec3> faces);
  MeshData(Bloc<float> vdata,Bloc<uint> attribute_widths);
  MeshData(Bloc<float> vdata,Bloc<uint> attribute_widths,Bloc<uint> elements);

  void draw(uint mode) const;

  static MeshData readOBJ(string path);
};

class Mesh : public Renderable, public virtual Spatial{
  void render(Render*) override;
public:
  MeshData mesh_data;
  Shader shader;

  Mesh(){}
  Mesh(const MeshData& md):mesh_data(md){}
};
