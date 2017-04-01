#include <cmath>

#include "light.h"

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{

	Scene* scene = getScene();
	vec3f direction = getDirection(P);
	isect i;
	ray lightRay(P, direction);
	bool hasOne = scene->intersect(lightRay, i);
	if (hasOne)
		return i.getMaterial().kt;
	else
		return vec3f(1, 1, 1);
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


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{

	Scene* scene = getScene();
	vec3f direction = getDirection(P);
	isect i;
	ray lightRay(P, direction);
	bool hasOne = scene->intersect(lightRay, i);
	if (hasOne)
		return i.getMaterial().kt;
	else
		return vec3f(1, 1, 1);
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

vec3f SpotLight::shadowAttenuation(const vec3f& P) const
{
	
	Scene* scene = getScene();
	vec3f direction = getDirection(P);
	isect i;
	ray lightRay(P, direction);
	bool hasOne = scene->intersect(lightRay, i);
	if (hasOne)
		return i.getMaterial().kt;
	else
		return vec3f(1, 1, 1);
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
		cout << "(" << x << "," << y << ")";
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

vec3f ShapeLight::shadowAttenuation(const vec3f& P) const
{

	Scene* scene = getScene();
	vec3f direction = getDirection(P);
	isect i;
	ray lightRay(P, direction);
	bool hasOne = scene->intersect(lightRay, i);
	if (hasOne)
		return i.getMaterial().kt;
	else
		return vec3f(1, 1, 1);
}
