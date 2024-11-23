#pragma once
#include <memory>
#include "util/vec.hpp"

struct Texture;

struct Material{
	std::shared_ptr<Texture> albedo_texture;
	fvec3 albedo{0.5,0.5,0.5};
	std::shared_ptr<Texture> normal_texture;
	std::shared_ptr<Texture> rds_texture;
	bool roughness_use_texture = false;
	bool diffuse_use_texture = false;
	bool specular_use_texture = false;
	float roughness{1};
	float diffuse{0.5};
	float specular{0.5};
};
