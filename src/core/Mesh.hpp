#pragma once
#include "Shader.hpp"
#include "util/Bloc.hpp"
#include "Object.hpp"
#include "Spatial.hpp"

struct Viewport;

class Mesh{
	GLuint VAO=0,VBO=0,EBO=0;
	GLuint elemcount=0;
	Mesh(GLuint VAO, GLuint VBO, GLuint EBO, GLuint elemcount): VAO(VAO), VBO(VBO), EBO(EBO), elemcount(elemcount){}
public:

	Mesh()=delete;
	Mesh(const Mesh&)=delete;
	Mesh(Mesh&&)=delete;
	Mesh& operator=(const Mesh&)=delete;
	Mesh& operator=(Mesh&&)=delete;

	~Mesh();

	// create a mesh from only vertex data; every 3 verts is a face
	static Mesh* create(Bloc<fvec3> verts);
	// create a mesh from vertex data and faces
	static Mesh* create(Bloc<fvec3> verts,Bloc<ivec3> faces);
	// create a mesh from vertex data with attributes; every 3 verts is a face
	static Mesh* create(Bloc<float> vdata,Bloc<uint> attribute_widths);
	// create a mesh from vertex data with attributes and faces;
	static Mesh* create(Bloc<float> vdata,Bloc<uint> attribute_widths,Bloc<uint> elements);

	void draw(GLenum mode) const;

	static Mesh* readOBJ(const std::string& path);
};

struct MeshObject : public Object{
	Transform transform;
	void render(const Viewport*);
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Shader> shader;

	MeshObject(){}
	MeshObject(const std::shared_ptr<Mesh>& md):mesh(md){}
};
