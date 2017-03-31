#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/scene.h"
#include "scene/ray.h"
#include "scene/material.h"
#include "fileio/bitmap.h"
#include <stack>

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    vec3f trace( Scene *scene, double x, double y );
	vec3f traceRay( Scene *scene, const ray& r, const vec3f& thresh, int depth ,stack<Material> materials);


	void getBuffer( unsigned char *&buf, int &w, int &h );
	double aspectRatio();
	void traceSetup( int w, int h );
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel( int i, int j );

	bool loadScene( char* fn );

	bool sceneLoaded();

	Scene* getScene() {
		return scene;
	}

	void setSupperSamping(bool value) { supperSampling = value; }
	bool getSupperSamping() { return supperSampling; }
	void setAdaptive(bool value) { adaptive = value; }
	bool getAdaptive() { return adaptive; }
	void setGridSize(int value) { gridSize = value; }
	void setJitter(bool value) { jitter = value; }
	bool getJitter() { return jitter; }

	//background image 
	void setUsingBackgroundImage(bool value) { usingBackgroundImage = value; }
	bool getUisingBackgroundImage() { return usingBackgroundImage; }
	void loadBackgroundImage(char* fn);

private:
	unsigned char *buffer;
	int buffer_width, buffer_height;
	int bufferSize;
	Scene *scene;

	bool m_bSceneLoaded;
	
	//anti-aliasing
	bool supperSampling;
	bool adaptive;
	bool jitter;
	int gridSize;
	
	//background image
	unsigned char* m_ucBackground;
	bool usingBackgroundImage;
	int m_nWidth;
	int m_nHeight;

	vec3f refractionDirection(vec3f& normal, vec3f& dir, double indexFrom, double indexTo);
};

#endif // __RAYTRACER_H__
