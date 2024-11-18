#pragma once
#include "core/Render.hpp"
#include "core/Object.hpp"

class OrbitCamera :public PerspectiveCamera, public Object{
	void onEscapePress();
public:
	double orbit_distance=5.0;
	double speed=0.003;
	void update() ;
	fmat4 getView() const override;
	void init();
	OrbitCamera();
};
