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
	} else {//the ray is inside the sphere
		i.t = t2;
		i.N = -r.at( t2 ).normalize();
	}

	return true;
}

vec3f Sphere::getTextureColor(vec3f& intersecPoint) {
	if (!scene->isTextureLoaded())
		return vec3f(0.0, 0.0, 0.0);
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

	vec3f x = (pole ^ dir).normalize();
	vec3f y = (dir ^ x).normalize();

	mat4f rotate(
		vec4f(x[0], x[1], x[2], 0.0f),
		vec4f(y[0], y[1], y[2], 0.0f),
		vec4f(dir[0], dir[1], dir[2], 0.0),
		vec4f(0.0, 0.0, 0.0, 1.0)
	);
	//cout << (equator * dir) / sin(phi);
	vec3f pxe(1.0, 0.0, 0.0);
	
	if (pxe * dir <= 0)
		u = 1 - u;

	setTextureNormal(u, v, rotate);
	return scene->getColor(u, v);
}

T Sphere::getPrimitiveT(const ray& r) {
	/*
	if (r.getPosition().length() < 1) {
		cout << "even more strange" << endl;
	}
	*/
	vec3f pos = transform->globalToLocalCoords(r.getPosition());
	
	vec3f dir = (transform->globalToLocalCoords(r.getPosition() + r.getDirection()) - pos).normalize();
	ray localRay(pos, dir);
	vec3f v = -localRay.getPosition();
	double b = v.dot(localRay.getDirection());
	double discriminant = b*b - v.dot(v) + 1;

	if (discriminant < RAY_EPSILON) {
		//cout << endl;
		return T();
	}

	discriminant = sqrt(discriminant);
	double t2 = b + discriminant;

	if (t2 <= RAY_EPSILON) {
		//cout << endl;
		return T();
	}

	double t1 = b - discriminant;
	//if (pos.length() < 1.0)
		//cout << "strange" << endl;
	//cout << "sphere:" << t1 << ",";
	if (t1 < RAY_EPSILON)
		return T();
	else
		return T(t1, t2);
}

