#pragma once
#ifndef _TRAINGLE_H_
#define _TRIANGLE_H_

#include "../scene/scene.h"

class Triangle
	: public MaterialSceneObject
{
public:
	Triangle(Scene *scene, Material *mat)
		: MaterialSceneObject(scene, mat)
	{
	}

	virtual bool intersectLocal(const ray& r, isect& i) const;
	virtual bool hasBoundingBoxCapability() const { return true; }
	virtual BoundingBox ComputeLocalBoundingBox()
	{
		BoundingBox localbounds;
		localbounds.min = minimum(position[0], position[1]);
		localbounds.min = minimum(localbounds.min, position[2]);

		localbounds.max = maximum(position[0], position[1]);
		localbounds.max = maximum(localbounds.max, position[2]);
		return localbounds;
	}

	vec3f position[3];
};

#endif // !_TRAINGLE_H_
