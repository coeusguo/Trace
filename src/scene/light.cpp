#include <cmath>

#include "light.h"

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
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
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0
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
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
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
