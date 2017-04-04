// The main ray tracer.

#include <Fl/fl_ask.h>
#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include <random>
#include <stack>

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
	stack<Material> material;

	double stepX = 1.0 / (double(gridSize) * buffer_width * 2);
	double stepY = 1.0 / (double(gridSize) * buffer_height * 2);
	if (supperSampling) {
		double deltaX = 0.0;
		double deltaY = 0.0;
		if (jitter) {
			deltaX = rand(re) - 1.0;
			deltaY = rand(re) - 1.0;
		}
		vec3f result(0.0, 0.0, 0.0);
		
		x = x - (gridSize / 2) * stepX + stepX * 0.5;
		y = y - (gridSize / 2) * stepY + stepY * 0.5;
		for (int Y = 1; Y <= gridSize; Y++) {
			for (int X = 1; X <= gridSize; X++) {
				scene->getCamera()->rayThrough(x + deltaX * stepX, y + deltaY * stepY, r);
				result += traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0, material).clamp();
				x += stepX;
			}
			y += stepY;
		}
		double number = 1.0/(gridSize * gridSize);
		result *= number;
		return result;
	}
	if (enableDepthOfField) {
		vec3f focalPoint = r.at(focalLength + 1);
		vec3f result(0.0, 0.0, 0.0);
		result = traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0, material).clamp();
		for (int i = 0; i < 50; i++) {
			double deltaX = rand(re) - 1.0;
			double deltaY = rand(re) - 1.0;
			//cout << "(" << deltaX << "," << deltaY << ")";
			vec3f newPoint = scene->getCamera()->getPlaneWorldCoords(x + deltaX *0.05 * apertureSize, y + deltaY * 0.05 * apertureSize);
			vec3f newDir = (focalPoint - newPoint).normalize();
			ray newRay(newPoint, newDir);
			result += traceRay(scene, newRay, vec3f(1.0, 1.0, 1.0), scene->getDepth() - 2, material).clamp();
		}
		for (int i = 0; i < 3; i++) {
			result[i] /= 51.0f;
			//cout << result[i] << ",";
		}
		return result;
	}
	return traceRay( scene, r, vec3f(1.0,1.0,1.0), 0 ,material).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay(Scene *scene, const ray& r,
	const vec3f& thresh, int depth,stack<Material> materials)
{
	isect i;
	if (depth > scene->getDepth())
		return vec3f(0.0, 0.0, 0.0);

	if (scene->intersect(r, i)) {
		const Material& m = i.getMaterial();
		vec3f normal = i.N;//the normal of the inter section point
		vec3f dir = r.getDirection();//the direction of the ray
		vec3f pos = r.at(i.t);//the position of the intersection point

		vec3f phong(0.0, 0.0, 0.0);
		vec3f refColor(0.0, 0.0, 0.0);
		vec3f refraColor(0.0, 0.0, 0.0);
		//cout << (-dir)*normal<<endl;
		//phong shading
		phong = m.shade(scene, r, i);
		
		//reflection
		vec3f reflecVec = r.getDirection() - r.getDirection() * normal * 2 * normal;
		reflecVec = reflecVec.normalize();
		ray newRay(pos, reflecVec);
		refColor = traceRay(scene, newRay, thresh, depth + 1,materials);
		refColor = prod(refColor, m.kr);

		//refraction
		//the matrial is transmissive if length > 0
		if (m.kt.length() > 0) {
			vec3f newDir;
			//stack is empty, air -> outermost object
			if (materials.empty()) {
				materials.push(m);
				//cout << m.id << ",";
				newDir = refractionDirection(normal, dir, 1.0, m.index);
			}
			else {//not empty, obj -> obj, obj->air
				Material& material = materials.top();
				if (material.id != m.id) {//differect object,the ray must be go the inner object
					materials.push(m);
					newDir = refractionDirection(normal, dir, material.index, m.index);
				}
				else {//the same object,the ray escape from the inner object to outer object or to the air
					materials.pop();
					if (materials.empty()) {//current object is the outermost object,obj -> air
						newDir = refractionDirection(normal, dir, material.index, 1.0);
					}
					else {//not the outermost object,inner object -> outer object
						Material& outer = materials.top();
						newDir = refractionDirection(normal, dir, material.index, outer.index);
					}
				}
			}
			ray newRay(pos, newDir);
			if(newDir.length()!=0)//no inner reflection
				refraColor = traceRay(scene, newRay, thresh, depth + 1, materials);
		}

		refraColor = prod(refraColor, m.kt);

		vec3f result = phong + refColor + refraColor;
		return result;
	}
	else {
		if (usingBackgroundImage && m_ucBackground && depth==0) {
			int x = r.getCoords()[0] * m_nWidth;
			int y = r.getCoords()[1] * m_nHeight;
			int index = (y * m_nWidth + x) * 3;
			//cout << "(" << r.getCoords()[0] << "," << r.getCoords()[1] << ")";
			return vec3f(m_ucBackground[index] / 255.0, m_ucBackground[index + 1] / 255.0, m_ucBackground[index + 2] / 255.0);
		}
		else {
			return vec3f(0.0, 0.0, 0.0);
		}
	}
}

RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;
	m_ucBackground = NULL;
	m_bSceneLoaded = false;
	supperSampling = false;
	adaptive = false;
	usingBackgroundImage = false;
	jitter = false;
	gridSize = 3;
	apertureSize = 2;
	focalLength = 2.0;
	enableDepthOfField = false;
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

void RayTracer::loadBackgroundImage(char* fn) {
	
	if (m_ucBackground)
		delete[]m_ucBackground;
	
	unsigned char*	data;
	int				width,height;

	if ((data = readBMP(fn, width, height)) == NULL)
	{
		fl_alert("Can't load bitmap file");
		return;
	}

	m_nWidth = width;
	m_nHeight = height;
	m_ucBackground = data;

}


vec3f RayTracer::refractionDirection(vec3f& normal, vec3f& dir, double indexFrom, double indexTo) {
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