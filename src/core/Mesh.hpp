#pragma once
#include "defs/gl_defs.hpp"
#include "util/Bloc.hpp"
#include "Object.hpp"
#include "Spatial.hpp"
#include <map>

struct UniformValue;
struct Viewport;
struct Shader;
struct Material;

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
	// one dimension must be 0; if the other 2 are positive, the plane will face the
	// positive direction of the 0 axis. e.g, makePlane({1,2,0}) faces +z.
	static Mesh* makePlane(fvec3 size);
};

struct MeshObject : public Object{
	Transform transform;
	void render(const Viewport*);
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Material> material;
	std::map<std::string,UniformValue> bound_uniforms;

	MeshObject(){}
	MeshObject(const std::shared_ptr<Mesh>& md):mesh(md){}
};
