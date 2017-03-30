// The main ray tracer.

#include <Fl/fl_ask.h>
#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include <random>

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.

vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
	scene->getCamera()->rayThrough(x, y, r);

	uniform_real_distribution<double> rand(0,2);
	static default_random_engine re;

	if (supperSampling) {
		double deltaX = 0.0;
		double deltaY = 0.0;
		if (jitter) {
			deltaX = rand(re) - 1.0;
			deltaY = rand(re) - 1.0;
		}
		vec3f result(0.0, 0.0, 0.0);
		double stepX = 1.0 / (double(gridSize) * buffer_width * 2);
		double stepY = 1.0 / (double(gridSize) * buffer_height * 2);
		x = x - (gridSize / 2) * stepX + stepX * 0.5;
		y = y - (gridSize / 2) * stepY + stepY * 0.5;
		for (int Y = 1; Y <= gridSize; Y++) {
			for (int X = 1; X <= gridSize; X++) {
				scene->getCamera()->rayThrough(x + deltaX * stepX, y + deltaY * stepY, r);
				result += traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0, false).clamp();
				x += stepX;
			}
			y += stepY;
		}
		double number = 1.0/(gridSize * gridSize);
		result *= number;
		return result;
	}
	
	return traceRay( scene, r, vec3f(1.0,1.0,1.0), 0 ,false).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay(Scene *scene, const ray& r,
	const vec3f& thresh, int depth,bool isInside)
{
	isect i;
	if (depth > scene->getDepth())
		return vec3f(0.0, 0.0, 0.0);

	if (scene->intersect(r, i)) {
		const Material& m = i.getMaterial();
		vec3f phong(0.0, 0.0, 0.0);
		vec3f refColor(0.0, 0.0, 0.0);
		vec3f refraColor(0.0, 0.0, 0.0);

		//phong shading
		phong = m.shade(scene, r, i);
		//cout << "(" << phong[0] << "," << phong[1] << "," << phong[2] << ")";
		vec3f normal = i.N;
		vec3f reflecVec = r.getDirection() - r.getDirection() * normal * 2 * normal;
		reflecVec = reflecVec.normalize();
		ray newRay(r.at(i.t), reflecVec);
		
		//reflection
		refColor = traceRay(scene, newRay, thresh, depth + 1,isInside);

		refColor = prod(refColor, m.kr);

		//the matrial is transmissive if length > 0
		if (i.getMaterial().kt.length() > 0) {
			double index = i.getMaterial().index;
			double indexAir = 1.0;

			//check whether the ray is inside the box
			vec3f pos = r.at(i.t);//the position of intersection point
			vec3f dir = r.getDirection();

			
			//the ray is perpendicuar to the surface

			if (!isInside) {//air->object
				
				if (abs(dir * normal) < RAY_EPSILON) {//dir is conside with normal
					ray newRay(pos, dir);
					refraColor = traceRay(scene, newRay, thresh, depth + 1,true);
				}
				else {

					double sinAir = sqrt(1 - pow(dir * normal, 2));
					double radAir = asin(sinAir);
					
					double sinTheta2 = sinAir / index;//index of air is 1.0,so omitted here
					double delta = radAir - asin(sinTheta2);
					double sinTheta1 = sin(delta);


					double fac = sinTheta2 / sinTheta1;
					vec3f newDir = -(normal + fac * (-dir));
					newDir = newDir.normalize();
					ray refraRay(pos, newDir);
					refraColor = traceRay(scene, refraRay, thresh, depth + 1, true);
					//fl_message();
				}
				

			}
			else {//obj->air
				if (abs(dir * normal) < RAY_EPSILON) {
					ray newRay(pos, dir);
					refraColor = traceRay(scene, newRay, thresh, depth + 1, false);
				}
				else {
					double critical = 1.0 / index;//totally inner reflection if sinTheta is greater than this
					double sinTheta1 = sqrt(1 - pow(dir * normal, 2));
					if (critical - sinTheta1 > RAY_EPSILON) {
						double sinAir = index * sinTheta1;
						double radAir = asin(sinAir);
						double sinTheta2 = sin(radAir - asin(sinTheta1));
						double sinTheta3 = sin(3.1416 - radAir);
						double fac = sinTheta3 / sinTheta2;
						vec3f newDir = -(-normal + fac * (-dir));
						newDir = newDir.normalize();
						ray refraRay(pos, newDir);
						refraColor = traceRay(scene, refraRay, thresh, depth + 1,false);
					}
					else {

						refraColor = vec3f(0, 0, 0);
					}
				}
			}
			
			
		}
		refraColor = prod(refraColor, m.kt);

		vec3f result = phong + refColor + refraColor;
		//cout << "(" << result[0] << "," << result[1] << "," << result[2] << ")";
		return result;
	}
	else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		return vec3f(0.0, 0.0, 0.0);
	}
}

RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;
	m_bSceneLoaded = false;
	supperSampling = false;
	adaptive = false;
	jitter = false;
	gridSize = 3;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col;

	if( !scene )
		return;

	
	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);
	
	col = trace( scene,x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}