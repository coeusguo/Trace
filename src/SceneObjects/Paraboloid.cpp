/*#include <cmath>

#include "Paraboloid.h"

bool Paraboloid::intersectLocal(const ray& r, isect& i) const
{
	i.obj = this;

	if (intersectCaps(r, i)) {
		isect ii;
		if (intersectBody(r, ii)) {
			if (ii.t < i.t) {
				i = ii;
				i.obj = this;
			}
		}
		return true;
	}
	else {
		return intersectBody(r, i);
	}
}


bool Paraboloid::intersectBody(const ray& r, isect& i) const
{
	vec3f d = r.getDirection();
	vec3f p = r.getPosition();

	double a = (d[0] * d[0]) + (d[1] * d[1]);
	double b = 2.0*((d[0] * p[0]) + (d[1] * p[1]));
	double c = (p[0] * p[0]) + (p[1] * p[1]) - (p[2] + d[2]);

	double disc = b*b - 4.0*a*c;

	if (disc <= 0.0) {
		return false;
	}

	disc = sqrt(disc);

	double t1 = (-b - disc) / (2.0 * a);
	double t2 = (-b + disc) / (2.0 * a);

	if (t2 < RAY_EPSILON) {
		return false;
	}

	if (t1 > RAY_EPSILON) {
		// Two intersections.
		vec3f P = r.at(t1);
		double z = P[2];
		if (z >= 0.0 && z <= height) {
			// It's okay.
			i.t = t1;
			i.N = vec3f(P[0], P[1],
				-(C*P[2] + (t_radius - b_radius)*t_radius / height)).normalize();


			return true;
		}
	}

	vec3f P = r.at(t2);
	double z = P[2];
	if (z >= 0.0 && z <= height) {
		i.t = t2;
		i.N = vec3f(P[0], P[1],
			-(C*P[2] + (t_radius - b_radius)*t_radius / height)).normalize();
		// In case we are _inside_ the _uncapped_ cone, we need to flip the normal.
		// Essentially, the cone in this case is a double-sided surface
		// and has _2_ normals

		if (!capped && (i.N).dot(r.getDirection()) > 0)
			i.N = -i.N;

		return true;
	}

	return false;
}

bool Paraboloid::intersectCaps(const ray& r, isect& i) const
{
	if (!capped) {
		return false;
	}

	double pz = r.getPosition()[2];
	double dz = r.getDirection()[2];

	if (0.0 == dz) {
		return false;
	}

	double t1;
	double t2;
	double r1;
	double r2;

	if (dz > 0.0) {
		t1 = (-pz) / dz;
		t2 = (height - pz) / dz;
		r1 = b_radius;
		r2 = t_radius;
	}
	else {
		t1 = (height - pz) / dz;
		t2 = (-pz) / dz;
		r1 = t_radius;
		r2 = b_radius;
	}

	if (t2 < RAY_EPSILON) {
		return false;
	}

	if (t1 >= RAY_EPSILON) {
		vec3f p(r.at(t1));
		if ((p[0] * p[0] + p[1] * p[1]) <= r1 * r1) {
			i.t = t1;
			if (dz > 0.0) {
				// Intersection with cap at z = 0.
				i.N = vec3f(0.0, 0.0, -1.0);
			}
			else {
				i.N = vec3f(0.0, 0.0, 1.0);
			}
			return true;
		}
	}

	vec3f p(r.at(t2));
	if ((p[0] * p[0] + p[1] * p[1]) <= r2 * r2) {
		i.t = t2;
		if (dz > 0.0) {
			// Intersection with interior of cap at z = 1.
			i.N = vec3f(0.0, 0.0, 1.0);
		}
		else {
			i.N = vec3f(0.0, 0.0, -1.0);
		}
		return true;
	}

	return false;
}
*/

#include <cmath>

#include "Paraboloid.h"

bool Paraboloid::intersectLocal(const ray& r, isect& i) const
{
	i.obj = this;

/*	if (intersectCaps(r, i)) {
		isect ii;
		if (intersectBody(r, ii)) {
			if (ii.t < i.t) {
				i = ii;
				i.obj = this;
			}
		}
		return true;
	}
	else {*/
		return intersectBody(r, i);
//	}
}

bool Paraboloid::intersectBody(const ray& r, isect& i) const
{
	/*
	vec3f p = r.getPosition();
	vec3f d = r.getDirection();

	double a = (d[0]*d[0]) + (d[1]*d[1]);
	double b = 2.0*((d[0]*p[0]) + (d[1]*p[1]));
	double c = (p[0]*p[0]) + (p[1]*p[1]) - (p[2] + d[2]);
	*/
	double x0 = r.getPosition()[0];
	double y0 = r.getPosition()[1];
	double x1 = r.getDirection()[0];
	double y1 = r.getDirection()[1];

	double a = x1*x1;
	double b = 2.0*(x0*x1);
	double c = x0*x0 + 2*(y0 + y1);
	/*
	if (0.0 == a) {
		// This implies that x1 = 0.0 and y1 = 0.0, which further
		// implies that the ray is aligned with the body of the cylinder,
		// so no intersection.
		return false;
	}
	*/
	double discriminant = b*b - 4.0*a*c;

	if (discriminant < 0.0) {
		return false;
	}

	discriminant = sqrt(discriminant);

	double t2 = (-b + discriminant) / (2.0 * a);

	if (t2 <= RAY_EPSILON) {
		return false;
	}

	double t1 = (-b - discriminant) / (2.0 * a);

	if (t1 > RAY_EPSILON) {
		// Two intersections.
		vec3f P = r.at(t1);
		double z = P[2];
		if (z >= 0.0 && z <= 1.0) {
			// It's okay.
			i.t = t1;
			i.N = vec3f(P[0], P[1], 0.0).normalize();
			return true;
		}
	}

	vec3f P = r.at(t2);
	double z = P[2];
	if (z >= 0.0 && z <= 1.0) {
		i.t = t2;

		vec3f normal(P[0], P[1], 0.0);
		// In case we are _inside_ the _uncapped_ cone, we need to flip the normal.
		// Essentially, the cone in this case is a double-sided surface
		// and has _2_ normals
		if (normal.dot(r.getDirection()) > 0)
			normal = -normal;

		i.N = normal.normalize();
		return true;
	}

	return false;
}
/*
bool Paraboloid::intersectCaps(const ray& r, isect& i) const
{
	if (!capped) {
		return false;
	}

	double pz = r.getPosition()[2];
	double dz = r.getDirection()[2];

	if (0.0 == dz) {
		return false;
	}

	double t1;
	double t2;

	if (dz > 0.0) {
		t1 = (-pz) / dz;
		t2 = (1.0 - pz) / dz;
	}
	else {
		t1 = (1.0 - pz) / dz;
		t2 = (-pz) / dz;
	}

	if (t2 < RAY_EPSILON) {
		return false;
	}

	if (t1 >= RAY_EPSILON) {
		vec3f p(r.at(t1));
		if ((p[0] * p[0] + p[1] * p[1]) <= 1.0) {
			i.t = t1;
			if (dz > 0.0) {
				// Intersection with cap at z = 0.
				i.N = vec3f(0.0, 0.0, -1.0);
			}
			else {
				i.N = vec3f(0.0, 0.0, 1.0);
			}
			return true;
		}
	}

	vec3f p(r.at(t2));
	if ((p[0] * p[0] + p[1] * p[1]) <= 1.0) {
		i.t = t2;
		if (dz > 0.0) {
			// Intersection with cap at z = 1.
			i.N = vec3f(0.0, 0.0, 1.0);
		}
		else {
			i.N = vec3f(0.0, 0.0, -1.0);
		}
		return true;
	}

	return false;
}*/