//
// scene.h
//
// The Scene class and the geometric types that it can contain.
//

#ifndef __SCENE_H__
#define __SCENE_H__

#include <list>
#include <algorithm>

using namespace std;

#include "ray.h"
#include "material.h"
#include "camera.h"
#include "../vecmath/vecmath.h"

class Light;
class Scene;

class SceneElement
{
public:
	virtual ~SceneElement() {}

	Scene *getScene() const { return scene; }

protected:
	SceneElement( Scene *s )
		: scene( s ) {}

    Scene *scene;
};

class BoundingBox
{
public:
	vec3f min;
	vec3f max;

	void operator=(const BoundingBox& target);

	// Does this bounding box intersect the target?
	bool intersects(const BoundingBox &target) const;
	
	// does the box contain this point?
	bool intersects(const vec3f& point) const;

	// if the ray hits the box, put the "t" value of the intersection
	// closest to the origin in tMin and the "t" value of the far intersection
	// in tMax and return true, else return false.
	bool intersect(const ray& r, double& tMin, double& tMax) const;
};

class TransformNode
{
protected:

    // information about this node's transformation
    mat4f    xform;
	mat4f    inverse;
	mat3f    normi;

    // information about parent & children
    TransformNode *parent;
    list<TransformNode*> children;
    
public:
   	typedef list<TransformNode*>::iterator          child_iter;
	typedef list<TransformNode*>::const_iterator    child_citer;

    ~TransformNode()
    {
        for(child_iter c = children.begin(); c != children.end(); ++c )
            delete (*c);
    }

    TransformNode *createChild(const mat4f& xform)
    {
        TransformNode *child = new TransformNode(this, xform);
        children.push_back(child);
        return child;
    }
    
    // Coordinate-Space transformation
    vec3f globalToLocalCoords(const vec3f &v)
    {
        return inverse * v;
    }

    vec3f localToGlobalCoords(const vec3f &v)
    {
        return xform * v;
    }

    vec4f localToGlobalCoords(const vec4f &v)
    {
        return xform * v;
    }

    vec3f localToGlobalCoordsNormal(const vec3f &v)
    {
        return (normi * v).normalize();
    }

protected:
    // protected so that users can't directly construct one of these...
    // force them to use the createChild() method.  Note that they CAN
    // directly create a TransformRoot object.
    TransformNode(TransformNode *parent, const mat4f& xform )
        : children()
    {
        this->parent = parent;
        if (parent == NULL)
            this->xform = xform;
        else
            this->xform = parent->xform * xform;
        
        inverse = this->xform.inverse();
        normi = this->xform.upper33().inverse().transpose();
    }
};

class TransformRoot : public TransformNode
{
public:
    TransformRoot()
        : TransformNode(NULL, mat4f()) {}
};

// A Geometry object is anything that has extent in three dimensions.
// It may not be an actual visible scene object.  For example, hierarchical
// spatial subdivision could be expressed in terms of Geometry instances.
class Geometry
	: public SceneElement
{
public:
    // intersections performed in the global coordinate space.
    virtual bool intersect(const ray&r, isect&i) const;
    
    // intersections performed in the object's local coordinate space
    // do not call directly - this should only be called by intersect()
	virtual bool intersectLocal( const ray& r, isect& i ) const;


	virtual bool hasBoundingBoxCapability() const;
	const BoundingBox& getBoundingBox() const { return bounds; }
	virtual void ComputeBoundingBox()
    {
        // take the object's local bounding box, transform all 8 points on it,
        // and use those to find a new bounding box.

        BoundingBox localBounds = ComputeLocalBoundingBox();
        
        vec3f min = localBounds.min;
		vec3f max = localBounds.max;

		vec4f v, newMax, newMin;

		v = transform->localToGlobalCoords( vec4f(min[0], min[1], min[2], 1) );
		newMax = v;
		newMin = v;
		v = transform->localToGlobalCoords( vec4f(max[0], min[1], min[2], 1) );
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords( vec4f(min[0], max[1], min[2], 1) );
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords( vec4f(max[0], max[1], min[2], 1) );
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords( vec4f(min[0], min[1], max[2], 1) );
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords( vec4f(max[0], min[1], max[2], 1) );
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords( vec4f(min[0], max[1], max[2], 1) );
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		v = transform->localToGlobalCoords( vec4f(max[0], max[1], max[2], 1) );
		newMax = maximum(newMax, v);
		newMin = minimum(newMin, v);
		
		bounds.max = vec3f(newMax);
		bounds.min = vec3f(newMin);
    }

    // default method for ComputeLocalBoundingBox returns a bogus bounding box;
    // this should be overridden if hasBoundingBoxCapability() is true.
    virtual BoundingBox ComputeLocalBoundingBox() { return BoundingBox(); }

    void setTransform(TransformNode *transform) { this->transform = transform; };
    
	Geometry( Scene *scene ) 
		: SceneElement( scene ) {}

protected:
	BoundingBox bounds;
    TransformNode *transform;
};

// A SceneObject is a real actual thing that we want to model in the 
// world.  It has extent (its Geometry heritage) and surface properties
// (its material binding).  The decision of how to store that material
// is left up to the subclass.
class SceneObject
	: public Geometry
{
public:
	virtual const Material& getMaterial() const = 0;
	virtual void setMaterial( Material *m ) = 0;

protected:
	SceneObject( Scene *scene )
		: Geometry( scene ) {}
};

// A simple extension of SceneObject that adds an instance of Material
// for simple material bindings.
class MaterialSceneObject
	: public SceneObject
{
public:
	virtual ~MaterialSceneObject() { if( material ) delete material; }

	virtual const Material& getMaterial() const { return *material; }
	virtual void setMaterial( Material *m )	{ material = m; }

protected:
	MaterialSceneObject( Scene *scene, Material *mat ) 
		: SceneObject( scene ), material( mat ) {}
    //	MaterialSceneObject( Scene *scene ) 
	//	: SceneObject( scene ), material( new Material ) {}

	Material *material;
};

class Scene
{
public:
	typedef list<Light*>::iterator 			liter;
	typedef list<Light*>::const_iterator 	cliter;

	typedef list<Geometry*>::iterator 		giter;
	typedef list<Geometry*>::const_iterator cgiter;

    TransformRoot transformRoot;

public:
	Scene() 
		: transformRoot(), objects(), lights() {
		constant = 0.2;
		linear = 0.025;
		quadric = 0.005;
	}
	virtual ~Scene();

	void add( Geometry* obj )
	{
		obj->ComputeBoundingBox();
		objects.push_back( obj );
	}
	void add( Light* light )
	{ lights.push_back( light ); }

	bool intersect( const ray& r, isect& i ) const;
	void initScene();

	list<Light*>::const_iterator beginLights() const { return lights.begin(); }
	list<Light*>::const_iterator endLights() const { return lights.end(); }
        
	Camera *getCamera() { return &camera; }

	//distance attenuation
	void setConstant(double value) { constant = value; }
	void setLinear(double value) { linear = value; }
	void setQuadric(double value) { quadric = value; }
	double getConstant() { return constant; }
	double getLinear() { return linear * 0.1; }
	double getQuadric() { return quadric * 0.01; }

	void setAmbientLight(double value) { ambientLight = value; }
	double getAmbientLight() { return ambientLight; }

	//recurtion depth
	void setDepth(int value) { depth = value; }
	int getDepth() { return depth; }
private:
    list<Geometry*> objects;
	list<Geometry*> nonboundedobjects;
	list<Geometry*> boundedobjects;
    list<Light*> lights;
    Camera camera;
	
	//distance attenuation
	double constant;
	double linear;
	double quadric;
	double depth;
	double ambientLight;

	// Each object in the scene, provided that it has hasBoundingBoxCapability(),
	// must fall within this bounding box.  Objects that don't have hasBoundingBoxCapability()
	// are exempt from this requirement.
	BoundingBox sceneBounds;
};

#endif // __SCENE_H__
