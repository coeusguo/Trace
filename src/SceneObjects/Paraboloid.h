#ifndef __PARABOLOID_H__
#define __PARABOLOID_H__

#include "../scene/scene.h"

class Paraboloid
	: public MaterialSceneObject
{
public:
	Paraboloid(Scene *scene, Material *mat, bool cap = true)
		: MaterialSceneObject(scene, mat), capped(cap)
	{
	}

	virtual bool intersectLocal(const ray& r, isect& i) const;
	virtual bool hasBoundingBoxCapability() const { return true; }

	virtual BoundingBox ComputeLocalBoundingBox()
	{
		BoundingBox localbounds;
		localbounds.min = vec3f(-1.0f, -1.0f, 0.0f);
		localbounds.max = vec3f(1.0f, 1.0f, 1.0f);
		return localbounds;
	}

	bool intersectBody(const ray& r, isect& i) const;
	//bool intersectCaps(const ray& r, isect& i) const;

protected:
	bool capped;
};

#endif // __PARABOLOID_H__