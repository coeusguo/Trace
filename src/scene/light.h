#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"
#include "../utils/kdtree.h"
#include <random>

class Light
	: public SceneElement
{
public:
	virtual vec3f shadowAttenuation(const vec3f& P,bool enableSoftShadow) const = 0;
	virtual double distanceAttenuation( const vec3f& P ) const = 0;
	virtual vec3f getColor( const vec3f& P ) const = 0;
	virtual vec3f getDirection( const vec3f& P ) const = 0;
	virtual void computePhotonMap() { return; }
	vector<photon*> getPhotonList() { return list; }
protected:
	Light( Scene *scene, const vec3f& col )
		: SceneElement( scene ), color( col ) {}
	vec3f 		color;
	vector<photon*> list;
	int number = 0;
	void forwardTracing(ray& r, photon* p, int depth,bool isInside);
	vec3f refractionDirection(vec3f& normal, vec3f& dir, double indexFrom, double indexTo);
};

class DirectionalLight
	: public Light
{
public:
	DirectionalLight( Scene *scene, const vec3f& orien, const vec3f& color )
		: Light( scene, color ), orientation( orien ) {}
	virtual vec3f shadowAttenuation(const vec3f& P, bool enableSoftShadow) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;
protected:
	vec3f 		orientation;
};

class PointLight
	: public Light
{
public:
	PointLight( Scene *scene, const vec3f& pos, const vec3f& color )
		: Light( scene, color ), position( pos ) {}
	virtual vec3f shadowAttenuation(const vec3f& P, bool enableSoftShadow) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;
	virtual void computePhotonMap();

protected:
	vec3f position;
};

class SpotLight
	: public Light
{
public:
	SpotLight(Scene *scene, const vec3f& pos, const vec3f& direction, const vec3f edgePlace, const vec3f& color)
		: Light(scene, color), position(pos), direction(direction) {
		angleCos = cos((edgePlace[0] / 180)*3.1416);
		//cout << "success" << endl;
	}
	virtual vec3f shadowAttenuation(const vec3f& P, bool enableSoftShadow) const;
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;

protected:
	vec3f position;
	vec3f direction;
	double angleCos;
};


class ShapeLight
	: public Light
{
public:
	static enum {TRIANGLE = 0,SQUARE,STAR};
	ShapeLight(Scene *scene, const vec3f& pos, const vec3f& direction, const vec3f& parms,const vec3f& up,const vec3f& shape, const vec3f& color)
		: Light(scene, color), position(pos), direction(direction), y(up){
		shapeSize = parms[0];
		depth = parms[1];
		powIndex = parms[2];
		x = y ^ direction;
		//cout << x[0] << "," << x[1] << "," << x[2] << endl;
		x = x.normalize();
		y = direction ^ x;
		y.normalize();
		currentType = STAR;
		//cout << x[0] << "," << x[1] << "," << x[2] << endl;
		//cout << direction[0] << "," << direction[1] << "," << direction[2] << endl;
		mat4f translate(
			vec4f(1.0f, 0.0f, 0.0f, -position[0]),
			vec4f(0.0f, 1.0f, 0.0f, -position[1]),
			vec4f(0.0f, 0.0f, 1.0f, -position[2]),
			vec4f(0.0f, 0.0f, 0.0f, 1.0f));

		mat4f rotate(
			vec4f(x[0], x[1], x[2], 0),
			vec4f(y[0], y[1], y[2], 0),
			vec4f(direction[0], direction[1], direction[2], 0),
			vec4f(0.0, 0.0, 0.0, 1.0)
		);

		mat4f projection(
			vec4f(1.0, 0.0, 0.0, 0.0),
			vec4f(0.0, 1.0, 0.0, 0.0),
			vec4f(0.0, 0.0, 1.0, 0.0),
			vec4f(0.0, 0.0, 1.0f/depth, 0.0)
		);
		setType(shape[0]);
		projectionMatrix = projection * rotate * translate;
	}
	virtual vec3f shadowAttenuation(const vec3f& P, bool enableSoftShadow) const;
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;
	void setType(int type) {
		if (type < 0 || type > 2)
			return;
		currentType = type;
	}

protected:
	vec3f position;
	vec3f direction;
	double shapeSize;
	double depth;
	int powIndex;
	int currentType;
private:
	bool triangleShape(float x,float y)const;
	bool squareShape(float x,float y)const;
	bool starShape(float x,float y)const;
	vec3f y;//local x y plane of the shape
	vec3f x;
	mat4f projectionMatrix;//project points in world coordinate system to the xy plane in local coordinate system
};

#endif // __LIGHT_H__
