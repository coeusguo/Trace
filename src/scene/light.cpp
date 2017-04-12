#include <cmath>

#include "light.h"

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P, bool enableSoftShadow) const
{

	Scene* scene = getScene();
	vec3f direction = getDirection(P);
	isect i;
	ray lightRay(P, direction);

	vec3f result(1.0, 1.0, 1.0);
	bool hasOne;
	if (scene->getEnableOctree())
		hasOne = scene->intersectBoundingBox(lightRay, i);
	else
		hasOne = scene->intersect(lightRay, i);

	if (hasOne)
		result = i.getMaterial().kt;

	//soft shadow
	if (enableSoftShadow) {
		uniform_real_distribution<double> rand(0, 2);
		static default_random_engine re;

		vec3f u = (direction ^ vec3f(0.0, 1.0, 0.0)).normalize();
		vec3f v = (u ^ direction).normalize();

		isect ni;

		for (int i = 0; i < 50; i++) {
			float deltaU = rand(re) - 1.0;
			float deltaV = rand(re) - 1.0;

			vec3f newDir = (direction + u * deltaU * 0.05 + v * deltaV * 0.05).normalize();
			ray newRay(P, newDir);

			if (scene->intersect(newRay, ni))
				result += ni.getMaterial().kt;
			else
				result += vec3f(1.0, 1.0, 1.0);
		}

		for (int i = 0; i < 3; i++)
			result[i] /= 51.0f;
	}
	return result;
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	double distance = (P - position).length();
	Scene* scene = getScene();
	double att = 1.0 / (scene->getConstant() + scene->getLinear() * distance + scene->getQuadric() * distance * distance);
	if (att > 1.0)
		return 1.0;
	else
		return att;
}

vec3f PointLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P, bool enableSoftShadow) const
{

	Scene* scene = getScene();
	vec3f direction = getDirection(P);
	isect i;
	ray lightRay(P, direction);

	vec3f result(1.0, 1.0, 1.0);
	bool hasOne;
	if (scene->getEnableOctree())
		hasOne = scene->intersectBoundingBox(lightRay, i);
	else
		hasOne = scene->intersect(lightRay, i);

	float distance = (position - P).length();
	if (hasOne && distance > i.t)
		result = i.getMaterial().kt;
	else
		result = color;

	//soft shadow
	if (enableSoftShadow) {
		uniform_real_distribution<double> rand(0, 2);
		static default_random_engine re;

		vec3f u = (direction ^ vec3f(0.0, 1.0, 0.0)).normalize();
		vec3f v = (u ^ direction).normalize();

		isect ni;

		for (int i = 0; i < 50; i++) {
			float deltaU = rand(re) - 1.0;
			float deltaV = rand(re) - 1.0;

			vec3f newDir = (direction + u * deltaU * 0.05 + v * deltaV * 0.05).normalize();
			ray newRay(P, newDir);
			
			if (scene->intersect(newRay, ni))
				result += ni.getMaterial().kt;
			else
				result += vec3f(1.0, 1.0, 1.0);
		}

		for (int i = 0; i < 3; i++)
			result[i] /= 51.0f;
	}
	return result;
}

vec3f SpotLight::getColor(const vec3f& P) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f SpotLight::getDirection(const vec3f& P) const
{
	return (position - P).normalize();
}

double SpotLight::distanceAttenuation(const vec3f& P) const {
	vec3f d = (P - position).normalize();
	double value = d * direction;
	if (value >= angleCos)
		return 1.0;
	else
		return 0.0;
}

vec3f SpotLight::shadowAttenuation(const vec3f& P, bool enableSoftShadow) const
{
	
	Scene* scene = getScene();
	vec3f direction = getDirection(P);
	isect i;
	ray lightRay(P, direction);

	vec3f result(1.0, 1.0, 1.0);
	bool hasOne;
	if (scene->getEnableOctree())
		hasOne = scene->intersectBoundingBox(lightRay, i);
	else
		hasOne = scene->intersect(lightRay, i);

	float distance = (position - P).length();
	if (hasOne && distance > i.t)
		result = i.getMaterial().kt;
	else
		result = color;

	//soft shadow
	if (enableSoftShadow) {
		uniform_real_distribution<double> rand(0, 2);
		static default_random_engine re;

		vec3f u = (direction ^ vec3f(0.0, 1.0, 0.0)).normalize();
		vec3f v = (u ^ direction).normalize();

		isect ni;

		for (int i = 0; i < 50; i++) {
			float deltaU = rand(re) - 1.0;
			float deltaV = rand(re) - 1.0;

			vec3f newDir = (direction + u * deltaU * 0.05 + v * deltaV * 0.05).normalize();
			ray newRay(P, newDir);

			if (scene->intersect(newRay, ni))
				result += ni.getMaterial().kt;
			else
				result += vec3f(1.0, 1.0, 1.0);
		}

		for (int i = 0; i < 3; i++)
			result[i] /= 51.0f;
	}
	return result;
}

vec3f ShapeLight::getColor(const vec3f& P) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f ShapeLight::getDirection(const vec3f& P) const
{
	return (position - P).normalize();
}

bool ShapeLight::squareShape(float x, float y)const {
	if (abs(x) <= shapeSize&&abs(y) <= shapeSize)
		return true;
	return false;
}

bool ShapeLight::triangleShape(float x, float y)const {

	bool state1 = (y <= 1.732 * x + shapeSize);
	bool state2 = (y <= -1.732 * x + shapeSize);
	bool state3 = (y >= -0.5 * shapeSize);
	
	if (state1 && state2 && state3) {
		return true;
		//cout << "(" << x << "," << y << ")";
	}
	//cout << "f";
	return false;
}

bool ShapeLight::starShape(float x, float y)const {

	bool state1 = triangleShape(x, y);
	bool state2 = (y >= 1.732 * x - shapeSize) && (y >= -1.732 * x - shapeSize) && (y <= 0.5 * shapeSize);

	if (state1 || state2)
		return true;
	return false;
}

double ShapeLight::distanceAttenuation(const vec3f& P) const {

	vec4f p(P);
	p = projectionMatrix * p;
	float x = p[0];
	float y = p[1];

	bool ok;
	switch (currentType) {
	case TRIANGLE:
		ok = triangleShape(x, y);
		break;
	case SQUARE:
		ok = squareShape(x, y);
		break;
	case STAR:
		ok = starShape(x, y);
		break;
	default:
		ok = false;
		break;
	}

	if (!ok)
		return 0.0;

	vec3f dir = (P - position).normalize();
	return pow(dir * direction, powIndex);
}

vec3f ShapeLight::shadowAttenuation(const vec3f& P, bool enableSoftShadow) const
{

	Scene* scene = getScene();
	vec3f direction = getDirection(P);
	isect i;
	ray lightRay(P, direction);

	vec3f result(1.0, 1.0, 1.0);
	bool hasOne = scene->intersect(lightRay, i);

	if (hasOne)
		result = i.getMaterial().kt;

	//soft shadow
	if (enableSoftShadow) {
		uniform_real_distribution<double> rand(0, 2);
		static default_random_engine re;

		vec3f u = (direction ^ vec3f(0.0, 1.0, 0.0)).normalize();
		vec3f v = (u ^ direction).normalize();

		isect ni;

		for (int i = 0; i < 50; i++) {
			float deltaU = rand(re) - 1.0;
			float deltaV = rand(re) - 1.0;

			vec3f newDir = (direction + u * deltaU * 0.01 + v * deltaV * 0.01).normalize();
			ray newRay(P, newDir);

			if (scene->intersect(newRay, ni))
				result += ni.getMaterial().kt;
			else
				result += vec3f(1.0, 1.0, 1.0);
		}

		for (int i = 0; i < 3; i++)
			result[i] /= 51.0f;
	}
	return result;
}

void PointLight::computePhotonMap() {
	
	for (float theta = 0; theta < 360; theta+=0.2) {
		for (float phi = 0; phi < 360; phi+=0.2) {
			float phiR = float(phi * 3.1416) / float(180);
			float thetaR = float(theta * 3.1416) / float(180);

			vec3f dir(sinf(phiR) * cosf(thetaR), sinf(phiR) * sinf(thetaR), cosf(phiR));
			photon* p = new photon;
			p->energy = color;
			p->direction = dir;
			ray r(position, dir);
			forwardTracing(r, p, 0, false);
		}
	}
	//cout << "number:" << number << endl;
}

void Light::forwardTracing(ray& r, photon* p, int depth,bool isInside) {
	isect i;
	if (depth > 5) {
		delete p;
		return;
	}

	bool isIntersect;

	isIntersect = scene->intersect(r, i);

	if (isIntersect) {
		const Material& m = i.getMaterial();
		bool isSpecular = m.kr.length() > 0;
		bool isTransmissive = m.kt.length() > 0;

		vec3f dir = r.getDirection();
		vec3f pos = r.at(i.t);
		vec3f normal = i.N;

		if (isSpecular) {
			photon* newP = p;
			if (isTransmissive) {
				newP = new photon;
				newP->direction = p->direction;
				newP->energy = p->energy;
			}

			vec3f newDir = ((-dir) * normal * 2 * normal + dir).normalize();
			ray newRay(pos, newDir);
			newP->energy = prod(prod(m.kr, newP->energy), m.kd);
			forwardTracing(newRay, newP, depth + 1, isInside);
		}

		if (isTransmissive) {
			vec3f newDir;
			if (isInside)
				newDir = refractionDirection(normal, dir, m.index, 1.0);
			else
				newDir = refractionDirection(normal, dir, 1.0, m.index);

			if (newDir.length() != 0) {
				ray newRay(pos, newDir);
				p->energy = prod(prod(m.kt, p->energy), color);
				p->direction = newDir;
				forwardTracing(newRay, p, depth + 1, !isInside);
			}
			else {
				delete p;
			}
		}

		if (!isTransmissive && !isSpecular) {//diffuse
			p->direction = dir;		
			p->position = pos;
			//cout << "(" << p->position << ")";
			number++;
			list.push_back(p);
			return;
		}
	}
	else {
		delete p;
	}
}

vec3f Light::refractionDirection(vec3f& normal, vec3f& dir, double indexFrom, double indexTo) {
	if (abs(abs(normal * dir) - 1) < RAY_EPSILON)
		return dir;

	double sinTheta1 = sqrt(1 - pow(normal * dir, 2));
	double sinTheta2 = (indexFrom * sinTheta1) / indexTo;
	double theta1 = asin(sinTheta1);
	double theta2 = asin(sinTheta2);
	double sinTheta3 = sin(abs(theta1 - theta2));

	if (indexFrom == indexTo) {
		return dir;
	}
	else if (indexFrom > indexTo) {//currentIndex is greater than the next index,should consider total inner reflection
		double critical = indexTo / indexFrom;//the value if sine,not radian or degree

		if (critical - sinTheta1 > RAY_EPSILON) {
			double sinAlpha = sin(3.1416 - theta2);
			double fac = sinAlpha / sinTheta3;//by sine rule

			return -(-dir * fac + (-normal)).normalize();
		}
		else {//total inner reflection,no refraction at all
			return vec3f(0.0, 0.0, 0.0);
		}
	}
	else {//indexTo > indexFrom
		double fac = sinTheta2 / sinTheta3;
		return (dir * fac + (-normal)).normalize();
	}
}
