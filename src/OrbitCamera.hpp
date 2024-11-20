#pragma once
#include "core/Render.hpp"
#include "core/Object.hpp"

class OrbitCamera :public PerspectiveCamera, public Object{
	void onEscapePress();
public:
	double orbit_distance=5.0;
	double speed=0.003;
	void update(Viewport*) ;
	dmat4 getView() const override;
	OrbitCamera();
	~OrbitCamera();
};
