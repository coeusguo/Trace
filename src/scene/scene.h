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
#include "BoundingBox.h"
#include "../utils/T.h"
#include "../utils/kdtree.h"
class Light;
class Scene;
class Octree;


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
	TransformNode() {
		numOfTimes = 0;
	}

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

	void motionBlurDeltaTransform() {
		xform = motionBlurDelta * xform;
		inverse = xform.inverse();
		numOfTimes++;
	}
	void resetMotionBlurTransform() {
		mat4f restore = mat4f::translate(vec3f(-0.01 * numOfTimes, -0.01 * numOfTimes, -0.01 * numOfTimes));
		xform = restore * xform;
		inverse = xform.inverse();
		numOfTimes = 0;
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
private:
	int numOfTimes;
	static mat4f motionBlurDelta;
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
	vec3f getBumpNormal() const{ return currentNormal; }
	void setTextureNormal(float x, float y, const mat4f& mat);

	//for CSG used only
	virtual T getPrimitiveT(const ray& r) {
		return T();
	}
	void motionBlurDeltaTranslate() {
		transform->motionBlurDeltaTransform();
	}
	void motionBlurTranslateRestore() {
		transform->resetMotionBlurTransform();
	}
protected:
	BoundingBox bounds;
    TransformNode *transform;

	vec3f currentNormal;
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
	virtual vec3f getTextureColor(vec3f& intersectPoint) {return material->kd;}
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
		depth = 0;
		ambientLightFactor = 0.20;
		ambientLight = vec3f(1.0, 1.0, 1.0);
		m_ucTextureImage = NULL;
		m_ucNormalMap = NULL;
		usingTexture = false;
		usingBump = false;
		enableCaustics = false;
		ot = NULL;
		kdtree = NULL;
	}
	virtual ~Scene();

	void add( Geometry* obj )
	{
		obj->ComputeBoundingBox();
		//cout << "add" << endl;
		objects.push_back( obj );
	}
	void add( Light* light )
	{ lights.push_back( light ); }

	bool intersect( const ray& r, isect& i ) const;
	bool intersectNonBounded(const ray& r, isect& i)const;
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
	double getQuadric() { return quadric * 0.05; }

	void setAmbientLightFactor(double value) { ambientLightFactor = value; }
	double getAmbientLightFactor() { return ambientLightFactor; }

	void setAmbientLight(vec3f ambient) { ambientLight = ambient; }
	vec3f getAmbientLight() { return ambientLight; }

	//recurtion depth
	void setDepth(int value) { depth = value; }
	int getDepth() { return depth; }

	//texture image
	void loadTextureImage(char* fn);
	bool getUsingTexture() { return usingTexture; }
	void setUsingTexture(bool value) { usingTexture = value; }
	void calcualteNormalMap(unsigned char* filteredMap);
	void setUsingBump(bool value) { usingBump = value; }
	bool getUsingBump() { return usingBump; }
	vec3f getColor(double u, double v);
	bool isTextureLoaded() { return m_ucTextureImage ? true : false; }
	vec3f getTextureNormal(float x, float y);
	void loadNormalMap(char* fname);
	void saveImage(char* fname);

	//height field
	void loadHeightFieldMap(char* fname);
	friend class Trimesh;
	
	//octree
	bool getEnableOctree() { return enableOctree; }
	void setEnableOctree(bool value) { enableOctree = value; }
	bool intersectBoundingBox(const ray& r, isect& i);
	void setOctreeDepth(int value) { octreeDepth = value; }
	void iniOctree(float x, float y, float z, float xs, float ys, float zs, int depth);

	//motion blur
	void motionBlurObjectsDeltaTransform() {
		for (giter i = objects.begin(); i != objects.end(); i++)
			(*i)->motionBlurDeltaTranslate();
	}
	void motionBlurObjectRestore() {
		for (giter i = objects.begin(); i != objects.end(); i++)
			(*i)->motionBlurTranslateRestore();
	}
	
	//caustic
	void setCaustic(bool value) { enableCaustics = value; }
	bool getEnableCaustic() { return enableCaustics; }
	void initPhotonMap();
	bool isPhotonMapLoaded() { return kdtree?true:false; }
	void getPhotons(vector<photon*>& plist,range* r);

private:
    list<Geometry*> objects;
	list<Geometry*> nonboundedobjects;
	list<Geometry*> boundedobjects;
	Octree * ot;
    list<Light*> lights;
    Camera camera;
	
	unsigned char* filter();
	//distance attenuation
	double constant;
	double linear;
	double quadric;
	double depth;
	double ambientLightFactor;
	vec3f ambientLight;
	//texture image
	unsigned char* m_ucTextureImage;
	unsigned char* m_ucNormalMap;
	bool usingTexture;
	int m_textureWidth;
	int m_textureHeight;
	bool usingBump;

	//octree
	bool enableOctree;
	int octreeDepth;

	//caustic
	bool enableCaustics;
	vector<photon*> photonList;
	kdNode* kdtree;


	// Each object in the scene, provided that it has hasBoundingBoxCapability(),
	// must fall within this bounding box.  Objects that don't have hasBoundingBoxCapability()
	// are exempt from this requirement.
	BoundingBox sceneBounds;
};


class OctNode : private BoundingBox {
private:
	OctNode* parent;
	OctNode* childrens[8];//an dynamic array of 8 childrens
	list<Geometry*> objects;
	bool empty;
	bool hasChildren;

protected:
	OctNode(OctNode* parent, vec3f& min, vec3f& max, int depth);
	~OctNode();
	void initChildren(int depth);
	typedef list<Geometry*>::const_iterator cgiter;
	void processObject(Geometry* object);
	bool intersectNode(const ray& r, isect& i);

public:
	bool isEmpty() { return empty; }
};

class Octree :public OctNode {
public:
	Octree(vec3f& min, vec3f& max, int depth) :OctNode(this, min, max, depth) {};
	~Octree();
	void processObjects(list<Geometry*> objects);
	void processOneObject(Geometry* object);
	bool intersectThis(const ray& r, isect& i);
};




#endif // __SCENE_H__
