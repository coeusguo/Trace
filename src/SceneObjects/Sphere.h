#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "../scene/scene.h"

class Sphere
	: public MaterialSceneObject
{
public:
	Sphere( Scene *scene, Material *mat )
		: MaterialSceneObject( scene, mat )
	{
	}
    
	virtual bool intersectLocal( const ray& r, isect& i ) const;
	virtual bool hasBoundingBoxCapability() const { return true; }
	virtual vec3f getTextureColor(vec3f& intersecPoint);
    virtual BoundingBox ComputeLocalBoundingBox()
    {
        BoundingBox localbounds;
		localbounds.min = vec3f(-1.0f, -1.0f, -1.0f);
		localbounds.max = vec3f(1.0f, 1.0f, 1.0f);
        return localbounds;
    }
	virtual T getPrimitiveT(const ray& r);

};
#endif // __SPHERE_H__
