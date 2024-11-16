#pragma once
#include "util/strings.hpp"
#include "util/vec.hpp"
#include "util/quat.hpp"

struct Transform{
	dvec3 position{0,0,0};
	dquat rotation=quatIdentity<double>();
	double scale=1.0;

	dmat4 toMatrix() const;
	dmat4 toInvMatrix() const;
	Transform inverse() const;

	dvec3 operator*(const dvec3& v) const;
	Transform operator*(const Transform& b) const;
	Transform operator/(const Transform& b) const;

	Transform& translate(const dvec3& delta);
	Transform& rotate(const dvec3& axis,double theta);
	Transform& scaleBy(double s);
};

//inverse transform
dvec3 operator/(const dvec3& v,const Transform& t);

struct Spatial{
	Transform transform;

	dvec3& position=transform.position;
	dquat& rotation=transform.rotation;
	double& scale=transform.scale;
};

STRUCT_TO_STRING(Transform,position,rotation,scale);
