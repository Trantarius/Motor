#include "Mesh.hpp"
#include "core/Shader.hpp"
#include "util/io.hpp"
#include "defs/gl_defs.hpp"
#include "Render.hpp"
#include <map>
#include "Material.hpp"
#include "util/print.hpp"

Mesh* Mesh::create(Bloc<fvec3> verts){
	GLuint VAO,VBO;

	assert(verts.size%3==0);
	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verts.size*sizeof(fvec3),verts,GL_STATIC_DRAW);

	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	std::unique_ptr<Mesh> mesh{new Mesh(VAO,VBO, 0, verts.size)};
	checkGLError();
	return mesh.release();
}

Mesh* Mesh::create(Bloc<fvec3> verts,Bloc<ivec3> faces){
	GLuint VAO,VBO,EBO;
	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,verts.size*sizeof(fvec3),verts,GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size*sizeof(ivec3),faces,GL_STATIC_DRAW);

	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	std::unique_ptr<Mesh> mesh{new Mesh(VAO,VBO,EBO,faces.size*3)};
	checkGLError();
	return mesh.release();
}
Mesh* Mesh::create(Bloc<float> vdata,Bloc<uint> attribute_widths){
	int stride=0;
	for(int n=0;n<attribute_widths.size;n++){
		stride+=attribute_widths[n];
	}

	assert(vdata.size%stride==0);
	int vertcount=vdata.size/stride;
	assert(vertcount%3==0);
	GLuint VAO,VBO;

	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,vdata.size*sizeof(float),vdata,GL_STATIC_DRAW);

	int off=0;
	for(int a=0;a<attribute_widths.size;a++){
		assert(attribute_widths[a]>0 && attribute_widths[a]<5);
		glVertexAttribPointer(a,attribute_widths[a],GL_FLOAT,GL_FALSE,stride*sizeof(float),(void*)(off*sizeof(float)));
		glEnableVertexAttribArray(a);
		off+=attribute_widths[a];
	}

	glBindVertexArray(0);
	std::unique_ptr<Mesh> mesh{new Mesh(VAO,VBO,0,vertcount)};
	checkGLError();
	return mesh.release();
}

Mesh* Mesh::create(Bloc<float> vdata,Bloc<uint> attribute_widths,Bloc<uint> elements){

	int stride=0;
	for(int n=0;n<attribute_widths.size;n++){
		stride+=attribute_widths[n];
	}
	assert(vdata.size%stride==0);
	GLuint VAO,VBO,EBO;

	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,vdata.size*sizeof(float),vdata,GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size*sizeof(uint),elements,GL_STATIC_DRAW);

	int off=0;
	for(int a=0;a<attribute_widths.size;a++){
		assert(attribute_widths[a]>0 && attribute_widths[a]<5);
		glVertexAttribPointer(a,attribute_widths[a],GL_FLOAT,GL_FALSE,stride*sizeof(float),(void*)(off*sizeof(float)));
		glEnableVertexAttribArray(a);
		off+=attribute_widths[a];
	}

	glBindVertexArray(0);
	std::unique_ptr<Mesh> mesh{new Mesh(VAO,VBO,EBO,elements.size)};
	checkGLError();
	return mesh.release();
}

Mesh::~Mesh(){
	if(VBO)
		glDeleteBuffers(1,&VBO);
	if(EBO)
		glDeleteBuffers(1,&EBO);
	if(VAO)
		glDeleteVertexArrays(1,&VAO);
}

void Mesh::draw(GLenum mode) const {
	glBindVertexArray(VAO);
	if(EBO==0){
		glDrawArrays(mode,0,elemcount);
	}else{
		glDrawElements(mode,elemcount,GL_UNSIGNED_INT,0);
	}
	glBindVertexArray(0);
	checkGLError();
}

void MeshObject::render(const Viewport* vp) {
	if(shader && mesh){
		shader->use();
		if(shader->hasUniform("model_matrix"))
			shader->setUniform("model_matrix",(fmat4)transform.toMatrix());
		if(shader->hasUniform("view_matrix"))
			shader->setUniform("view_matrix",(fmat4)vp->view_matrix);
		if(shader->hasUniform("projection_matrix"))
			shader->setUniform("projection_matrix",(fmat4)vp->projection_matrix);

		if(material){
			if(shader->hasUniform("enable_albedo_texture"))
				shader->setUniform("enable_albedo_texture", (bool)(material->albedo_texture));
			if(shader->hasUniform("albedo_texture") && material->albedo_texture)
				shader->setUniformTexture("albedo_texture", material->albedo_texture->getID());
			if(shader->hasUniform("albedo"))
				shader->setUniform("albedo",material->albedo);
			if(shader->hasUniform("enable_normal_texture"))
				shader->setUniform("enable_normal_texture", (bool)(material->normal_texture));
			if(shader->hasUniform("normal_texture") && material->normal_texture)
				shader->setUniformTexture("normal_texture",material->normal_texture->getID());
			if(shader->hasUniform("enable_roughness_texture"))
				shader->setUniform("enable_roughness_texture", (bool)(material->rds_texture && material->roughness_use_texture));
			if(shader->hasUniform("enable_diffuse_texture"))
				shader->setUniform("enable_diffuse_texture", (bool)(material->rds_texture && material->diffuse_use_texture));
			if(shader->hasUniform("enable_specular_texture"))
				shader->setUniform("enable_specular_texture", (bool)(material->rds_texture && material->specular_use_texture));
			if(shader->hasUniform("rds_texture") && material->rds_texture)
				shader->setUniformTexture("rds_texture", material->rds_texture->getID());
			if(shader->hasUniform("roughness"))
				shader->setUniform("roughness", material->roughness);
			if(shader->hasUniform("diffuse"))
				shader->setUniform("diffuse", material->diffuse);
			if(shader->hasUniform("specular"))
				shader->setUniform("specular", material->specular);
		}else{
			if(shader->hasUniform("enable_albedo_texture"))
				shader->setUniform("enable_albedo_texture", false);
			if(shader->hasUniform("albedo"))
				shader->setUniform("albedo",fvec3(0.5,0.5,0.5));
			if(shader->hasUniform("enable_normal_texture"))
				shader->setUniform("enable_normal_texture", false);
			if(shader->hasUniform("enable_roughness_texture"))
				shader->setUniform("enable_roughness_texture", false);
			if(shader->hasUniform("enable_diffuse_texture"))
				shader->setUniform("enable_diffuse_texture", false);
			if(shader->hasUniform("enable_specular_texture"))
				shader->setUniform("enable_specular_texture", false);
			if(shader->hasUniform("roughness"))
				shader->setUniform("roughness", (float)(1.0));
			if(shader->hasUniform("diffuse"))
				shader->setUniform("diffuse", (float)(0.5));
			if(shader->hasUniform("specular"))
				shader->setUniform("specular", (float)(0.5));
		}

		if(shader->hasUniform("material_buffer"))
			shader->setUniformTexture("material_buffer", vp->getMaterialBuffer());

		for(const std::pair<std::string,UniformValue>& pr : bound_uniforms){
			if(shader->hasUniform(pr.first))
				shader->setUniformValue(pr.first, pr.second);
		}

		mesh->draw(GL_TRIANGLES);
	}
}

Mesh* Mesh::makePlane(fvec3 size){
	std::vector<float> verts;
	if(size.x==0){
		//CCW order puts front face facing +x
		verts.insert(verts.end(),{0,-size.y/2,-size.z/2});
		verts.insert(verts.end(),{0,0});
		verts.insert(verts.end(),{((size.y<0)!=(size.z<0)?-1.0f:1.0f),0,0});
		verts.insert(verts.end(),{0, size.y/2,-size.z/2});
		verts.insert(verts.end(),{1,0});
		verts.insert(verts.end(),{((size.y<0)!=(size.z<0)?-1.0f:1.0f),0,0});
		verts.insert(verts.end(),{0,-size.y/2, size.z/2});
		verts.insert(verts.end(),{0,1});
		verts.insert(verts.end(),{((size.y<0)!=(size.z<0)?-1.0f:1.0f),0,0});

		verts.insert(verts.end(),{0, size.y/2, size.z/2});
		verts.insert(verts.end(),{1,1});
		verts.insert(verts.end(),{((size.y<0)!=(size.z<0)?-1.0f:1.0f),0,0});
		verts.insert(verts.end(),{0,-size.y/2, size.z/2});
		verts.insert(verts.end(),{0,1});
		verts.insert(verts.end(),{((size.y<0)!=(size.z<0)?-1.0f:1.0f),0,0});
		verts.insert(verts.end(),{0, size.y/2,-size.z/2});
		verts.insert(verts.end(),{1,0});
		verts.insert(verts.end(),{((size.y<0)!=(size.z<0)?-1.0f:1.0f),0,0});
	}
	else if(size.y==0){
		//CCW order puts front face facing +y
		// this looks a little different because we have to pretend z is before x
		verts.insert(verts.end(),{-size.x/2, 0,-size.z/2});
		verts.insert(verts.end(),{0,0});
		verts.insert(verts.end(),{0,((size.x<0)!=(size.z<0)?-1.0f:1.0f),0});
		verts.insert(verts.end(),{-size.x/2, 0, size.z/2});
		verts.insert(verts.end(),{1,0});
		verts.insert(verts.end(),{0,((size.x<0)!=(size.z<0)?-1.0f:1.0f),0});
		verts.insert(verts.end(),{ size.x/2, 0,-size.z/2});
		verts.insert(verts.end(),{0,1});
		verts.insert(verts.end(),{0,((size.x<0)!=(size.z<0)?-1.0f:1.0f),0});

		verts.insert(verts.end(),{ size.x/2, 0, size.z/2});
		verts.insert(verts.end(),{1,1});
		verts.insert(verts.end(),{0,((size.x<0)!=(size.z<0)?-1.0f:1.0f),0});
		verts.insert(verts.end(),{ size.x/2, 0,-size.z/2});
		verts.insert(verts.end(),{0,1});
		verts.insert(verts.end(),{0,((size.x<0)!=(size.z<0)?-1.0f:1.0f),0});
		verts.insert(verts.end(),{-size.x/2, 0, size.z/2});
		verts.insert(verts.end(),{1,0});
		verts.insert(verts.end(),{0,((size.x<0)!=(size.z<0)?-1.0f:1.0f),0});
	}
	else if(size.z==0){
		//CCW order puts front face facing +z
		verts.insert(verts.end(),{-size.x/2,-size.y/2, 0});
		verts.insert(verts.end(),{0,0});
		verts.insert(verts.end(),{0,0,((size.x<0)!=(size.y<0)?-1.0f:1.0f)});
		verts.insert(verts.end(),{ size.x/2,-size.y/2, 0});
		verts.insert(verts.end(),{1,0});
		verts.insert(verts.end(),{0,0,((size.x<0)!=(size.y<0)?-1.0f:1.0f)});
		verts.insert(verts.end(),{-size.x/2, size.y/2, 0});
		verts.insert(verts.end(),{0,1});
		verts.insert(verts.end(),{0,0,((size.x<0)!=(size.y<0)?-1.0f:1.0f)});

		verts.insert(verts.end(),{ size.x/2, size.y/2, 0});
		verts.insert(verts.end(),{1,1});
		verts.insert(verts.end(),{0,0,((size.x<0)!=(size.y<0)?-1.0f:1.0f)});
		verts.insert(verts.end(),{-size.x/2, size.y/2, 0});
		verts.insert(verts.end(),{0,1});
		verts.insert(verts.end(),{0,0,((size.x<0)!=(size.y<0)?-1.0f:1.0f)});
		verts.insert(verts.end(),{ size.x/2,-size.y/2, 0});
		verts.insert(verts.end(),{1,0});
		verts.insert(verts.end(),{0,0,((size.x<0)!=(size.y<0)?-1.0f:1.0f)});
	}
	Bloc<float> bverts{verts.data(), verts.size()};
	std::vector<uint> attribute_widths{3,2,3}; // vertex, UV, normal
	Bloc<uint> widths{attribute_widths.data(), attribute_widths.size()};
	return create(bverts,widths);
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

Mesh* Mesh::readOBJ(const std::string& path){
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
	return Mesh::create(vdata_bloc,attribute_widths_bloc,elements_bloc);
}
