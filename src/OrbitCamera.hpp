#pragma once
#include "core/Render.hpp"

class OrbitCamera :public PerspectiveCamera, public std::enable_shared_from_this<OrbitCamera>{
	void onEscapePress();
public:
	double orbit_distance=5.0;
	double speed=0.00001;
	void update() ;
	fmat4 getView() const override;
	void init();
};
