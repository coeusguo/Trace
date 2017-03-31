#include <cmath>

#include "Sphere.h"

bool Sphere::intersectLocal( const ray& r, isect& i ) const
{
	vec3f v = -r.getPosition();
	double b = v.dot(r.getDirection());
	double discriminant = b*b - v.dot(v) + 1;

	if( discriminant < 0.0 ) {
		return false;
	}

	discriminant = sqrt( discriminant );
	double t2 = b + discriminant;

	if( t2 <= RAY_EPSILON ) {
		return false;
	}

	i.obj = this;

	double t1 = b - discriminant;

	if( t1 > RAY_EPSILON ) { //the ray is outside the sphere
		i.t = t1;
		i.N = r.at( t1 ).normalize();
	} else {//the rat is inside the sphere
		i.t = t2;
		i.N = -r.at( t2 ).normalize();
	}

	return true;
}

vec3f Sphere::getTextureColor(vec3f& intersecPoint) {
	vec3f localPoint = transform->globalToLocalCoords(intersecPoint);
	vec3f dir = localPoint.normalize();
	vec3f pole(0.0, 1.0, 0.0);
	vec3f equator(0.0, 0.0, 1.0);

	double phi = acos(-dir * pole);
	double v = phi / 3.1415926536;
	double u;
	
	double fac = (equator * dir) / sin(phi);
	if (1 + fac < RAY_EPSILON)
		u = 1;
	else if (1 - fac < RAY_EPSILON)
		u = 0;
	else
		u = acos((equator * dir) / sin(phi)) / (2 * 3.1416);

	//cout << (equator * dir) / sin(phi);
	vec3f pxe(1.0, 0.0, 0.0);
	
	if (pxe * dir <= 0)
		u = 1 - u;
	return scene->getColor(u, v);
}
