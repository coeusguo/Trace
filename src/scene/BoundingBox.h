#pragma once
#ifndef _BOUNDINGBOX_H_
#define _BOUNDINGBOX_H_

#include "../vecmath/vecmath.h"
#include "ray.h"

class BoundingBox
{
public:
	vec3f min;
	vec3f max;

	void operator=(const BoundingBox& target);

	// Does this bounding box intersect the target?
	bool intersects(const BoundingBox &target) const;

	// does the box contain this point?
	bool intersects(const vec3f& point) const;

	// if the ray hits the box, put the "t" value of the intersection
	// closest to the origin in tMin and the "t" value of the far intersection
	// in tMax and return true, else return false.
	bool intersect(const ray& r, double& tMin, double& tMax) const;
};
#endif // !_BOUNDINGBOX_H_
