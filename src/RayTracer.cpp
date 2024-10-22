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
	if (adaptive) {//adaptive supper sampling
		//cout << "wtf" << endl;
		ray topLeft(r); ray topRight(r); ray bottomLeft(r); ray bottomRight(r);
		double width = double(1) / double(buffer_width);
		double height = double(1) / double(buffer_height);
		getFiveRays(r, topLeft, topRight, bottomLeft, bottomRight, width, height);
		return adaptiveSuperSampling(r, topLeft, topRight, bottomLeft, bottomRight, 0);
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

	if (enableMotionBlur) {
		//cout << "?" << endl;
		vec3f result(0.0, 0.0, 0.0);
		for (int i = 0; i < 50; i++) {
			scene->motionBlurObjectsDeltaTransform();
			result += traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0, material).clamp();
		}
		scene->motionBlurObjectRestore();
		result /= 50;
		return result;
	}

	return traceRay( scene, r, vec3f(1.0,1.0,1.0), 0 ,material).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay(Scene *scene, ray& r,
	const vec3f& thresh, int depth,stack<Material> materials)
{
	isect i;
	if (depth > scene->getDepth())
		return vec3f(0.0, 0.0, 0.0);

	bool isIntersect;
	if (scene->getEnableOctree()) {
		isIntersect = scene->intersectBoundingBox(r, i);
		//cout << "isIntersect" << isIntersect << endl;
	}
	else
		isIntersect = scene->intersect(r, i);

	if (isIntersect) {
		//cout << i.getMaterial().kd << endl;
		const Material& m = i.getMaterial();
		vec3f normal = i.N;//the normal of the inter section point
		vec3f dir = r.getDirection();//the direction of the ray
		vec3f pos = r.at(i.t);//the position of the intersection point

		vec3f phong(0.0, 0.0, 0.0);
		vec3f refColor(0.0, 0.0, 0.0);
		vec3f refraColor(0.0, 0.0, 0.0);
		//cout << (-dir)*normal<<endl;
		//phong shading
		//cout << "?";
		phong = m.shade(scene, r, i, enableSoftShadow);
		//cout << "?";
		if (phong.length() / (depth + 1) > threshold) {
			//reflection
			vec3f reflecVec = r.getDirection() - r.getDirection() * normal * 2 * normal;
			reflecVec = reflecVec.normalize();
			ray newRay(pos, reflecVec);

			refColor = traceRay(scene, newRay, thresh, depth + 1, materials);

			//glossy reflection
			if (enableGlossy) {
				uniform_real_distribution<double> rand(0, 2);
				static default_random_engine re;

				vec3f u = (reflecVec ^ normal).normalize();
				vec3f v = (u ^ reflecVec).normalize();
				for (int i = 0; i < 50; i++) {
					float deltaU = rand(re) - 1.0;
					float deltaV = rand(re) - 1.0;

					vec3f newReflecVec = (reflecVec + u * deltaU * 0.05 + v * deltaV * 0.05).normalize();
					ray newRay(pos, newReflecVec);
					refColor += traceRay(scene, newRay, thresh, scene->getDepth() - 2, materials);
				}
				for (int i = 0; i < 3; i++)
					refColor[i] /= 51.0f;
			}


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
				if (newDir.length() != 0)//no inner reflection
					refraColor = traceRay(scene, newRay, thresh, depth + 1, materials);
			}

			refraColor = prod(refraColor, m.kt);
		}
		vec3f result = phong + refColor + refraColor;
		return result;
	}
	else {
		bool ok = true;
		if (depth > 0) {
			ok = scene->getCamera()->resetCoords(r);
		}
		if (usingBackgroundImage && m_ucBackground && ok) {
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
	threshold = 0.0;
	enableDepthOfField = false;
	enableGlossy = false;
	enableSoftShadow = false;
	enableMotionBlur = false;
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

void RayTracer::getFiveRays(ray&center,ray& topLeft, ray& topRight, ray& bottomleft, ray& bottomRight,double width,double height) {


	double* coords = center.getCoords();

	scene->getCamera()->rayThrough(coords[0] - width / double(2), coords[1] - height / double(2), bottomleft);
	scene->getCamera()->rayThrough(coords[0] + width / double(2), coords[1] - height / double(2), bottomRight);
	scene->getCamera()->rayThrough(coords[0] - width / double(2), coords[1] + height / double(2), topLeft);
	scene->getCamera()->rayThrough(coords[0] + width / double(2), coords[1] + height / double(2), topRight);
}

vec3f RayTracer::adaptiveSuperSampling(ray& center,ray& topLeft, ray& topRight, ray& bottomleft, ray& bottomRight,int depth) {
	if (depth == 3)
		return vec3f(0.0, 0.0, 0.0);
	stack<Material> material;

	vec3f colorCenter = traceRay(scene, center, vec3f(1.0, 1.0, 1.0), scene->getDepth() - 2, material).clamp();
	vec3f colorTopLeft = traceRay(scene, topLeft, vec3f(1.0, 1.0, 1.0), scene->getDepth() - 2, material).clamp();
	vec3f colorTopRight = traceRay(scene, topRight, vec3f(1.0, 1.0, 1.0), scene->getDepth() - 2, material).clamp();
	vec3f colorBottomLeft= traceRay(scene, bottomleft, vec3f(1.0, 1.0, 1.0), scene->getDepth() - 2, material).clamp();
	vec3f colorBottomRight = traceRay(scene, bottomRight, vec3f(1.0, 1.0, 1.0), scene->getDepth() - 2, material).clamp();
	bool statements[4];
	statements[0] = (colorCenter - colorTopLeft).length() > 0.1;
	statements[1] = (colorCenter - colorTopRight).length() > 0.1;
	statements[2] = (colorCenter - colorBottomLeft).length() > 0.1;
	statements[3] = (colorCenter - colorBottomRight).length() > 0.1;

	double pixelWidth = double(1) / double(buffer_width);
	double pixelHeight = double(1) / double(buffer_height);

	double level = pow(2, depth + 1);
	double width = pixelWidth / double(level);
	double height = pixelHeight / double(level);

	ray copyCenter(center);
	
	vec3f result(0.0, 0.0, 0.0);
	int numOfDiff = 0;
	if (statements[0]) {
		//cout << "?" << endl;
		center.getCoords()[0] = copyCenter.getCoords()[0] - width / double(2);
		center.getCoords()[1] = copyCenter.getCoords()[1] + height / double(2);
		getFiveRays(center, topLeft, topRight, bottomleft, bottomRight, width, height);
		vec3f newColor = adaptiveSuperSampling(center, topLeft, topRight, bottomleft, bottomRight, depth + 1);
		if (newColor.length() != 0)
			colorTopLeft = newColor;
	}

	if (statements[1]) {
		//cout << "??" << endl;
		center.getCoords()[0] = copyCenter.getCoords()[0] + width / double(2);
		center.getCoords()[1] = copyCenter.getCoords()[1] + height / double(2);
		getFiveRays(center, topLeft, topRight, bottomleft, bottomRight, width, height);
		vec3f newColor = adaptiveSuperSampling(center, topLeft, topRight, bottomleft, bottomRight, depth + 1);
		if (newColor.length() != 0)
			colorTopRight = newColor;
	}

	if (statements[2]) {
		//cout << "???" << endl;
		center.getCoords()[0] = copyCenter.getCoords()[0] - width / double(2);
		center.getCoords()[1] = copyCenter.getCoords()[1] - height / double(2);
		getFiveRays(center, topLeft, topRight, bottomleft, bottomRight, width, height);
		vec3f newColor = adaptiveSuperSampling(center, topLeft, topRight, bottomleft, bottomRight, depth + 1);
		if (newColor.length() != 0)
			colorBottomLeft = newColor;
	}

	if (statements[3]) {
		//cout << "????" << endl;
		center.getCoords()[0] = copyCenter.getCoords()[0] + width / double(2);
		center.getCoords()[1] = copyCenter.getCoords()[1] - height / double(2);
		getFiveRays(center, topLeft, topRight, bottomleft, bottomRight, width, height);
		vec3f newColor = adaptiveSuperSampling(center, topLeft, topRight, bottomleft, bottomRight, depth + 1);
		if (newColor.length() != 0) {
			colorBottomRight = newColor;
			//cout << "sss" << endl;
		}
	}

	
	result = (colorBottomRight + colorBottomLeft + colorTopLeft + colorTopRight) / double(4);
	return result;
}