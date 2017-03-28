#include <cmath>
#include <assert.h>

#include "Box.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.
	BoundingBox box;
	vec3f min(-0.5, -0.5, -0.5);
	vec3f max(0.5, 0.5, 0.5);
	box.min = min;
	box.max = max;

	double tmin, tmax;
	if (!box.intersect(r, tmin, tmax))
		return false;


	if (tmin <= RAY_EPSILON) {//the origin of the ray is inside the box or on one side of the box
		tmin = tmax;
	}
	if (tmax <= RAY_EPSILON)//the origin of the ray is really on the box
		return false;

	if (tmax - tmin <= RAY_EPSILON)//the ray is tanget to the box
		return false;

	i.t = tmin;

	vec3f point = r.at(tmin);

	if (abs(point[2] - 0.5) < RAY_EPSILON)
		i.N = vec3f(0, 0, 1.0);
	else if (abs(point[1] - 0.5) < RAY_EPSILON)
		i.N = vec3f(0, 1.0, 0);
	else if (abs(point[2] + 0.5) < RAY_EPSILON)
		i.N = vec3f(0, 0, -1.0);
	else if (abs(point[1] + 0.5) < RAY_EPSILON)
		i.N = vec3f(0, -1.0, 0);
	else if (abs(point[0] - 0.5) < RAY_EPSILON)
		i.N = vec3f(1.0, 0, 0);
	else
		i.N = vec3f(-1.0, 0, 0);

	i.obj = this;
	return true;
}
