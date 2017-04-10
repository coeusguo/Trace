/*#ifndef __HYPERBOLIC_H__
#define __HYPERBOLIC_H__

#include "../scene/scene.h"

class Hyperbolic
	: public MaterialSceneObject
{
public:
	Hyperbolic(Scene *scene, Material *mat,
		double h = 1.0, double br = 1.0, double tr = 0.0,
		bool cap = false)
		: MaterialSceneObject(scene, mat)
	{
		height = h;
		b_radius = (br < 0.0f) ? (-br) : (br);
		t_radius = (tr < 0.0f) ? (-tr) : (tr);
		capped = cap;

		computeABC();
	}

	virtual bool intersectLocal(const ray& r, isect& i) const;
	virtual bool hasBoundingBoxCapability() const { return true; }

	virtual BoundingBox ComputeLocalBoundingBox()
	{
		BoundingBox localbounds;
		double biggest_radius = (b_radius > t_radius) ? (b_radius) : (t_radius);

		localbounds.min = vec3f(-biggest_radius, -biggest_radius, (height < 0.0f) ? (height) : (0.0f));
		localbounds.max = vec3f(biggest_radius, biggest_radius, (height < 0.0f) ? (0.0f) : (height));
		return localbounds;
	}

	bool intersectBody(const ray& r, isect& i) const;
	bool intersectCaps(const ray& r, isect& i) const;


protected:
	void computeABC()
	{
		A = b_radius * b_radius;
		B = 2.0 * b_radius * (t_radius - b_radius) / height;
		C = (t_radius - b_radius) / height;
		C = C * C;
	}

	bool capped;
	double height;
	double b_radius;
	double t_radius;

	double A;
	double B;
	double C;

};

#endif // __HYPERBOLIC_H__
*/


#ifndef __HYPERBOLIC_H__
#define __HYPERBOLIC_H__

#include "../scene/scene.h"

class Hyperbolic
	: public MaterialSceneObject
{
public:
	Hyperbolic(Scene *scene, Material *mat, bool cap = true)
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

#endif // __HYPERBOLIC_H__
#pragma once