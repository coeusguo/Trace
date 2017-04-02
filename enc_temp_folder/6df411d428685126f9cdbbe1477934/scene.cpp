#include <cmath>

#include "scene.h"
#include "light.h"
#include "../ui/TraceUI.h"
#include "../SceneObjects/trimesh.h"

extern TraceUI* traceUI;

void BoundingBox::operator=(const BoundingBox& target)
{
	min = target.min;
	max = target.max;
}

// Does this bounding box intersect the target?
bool BoundingBox::intersects(const BoundingBox &target) const
{
	return ((target.min[0] - RAY_EPSILON <= max[0]) && (target.max[0] + RAY_EPSILON >= min[0]) &&
			(target.min[1] - RAY_EPSILON <= max[1]) && (target.max[1] + RAY_EPSILON >= min[1]) &&
			(target.min[2] - RAY_EPSILON <= max[2]) && (target.max[2] + RAY_EPSILON >= min[2]));
}

// does the box contain this point?
bool BoundingBox::intersects(const vec3f& point) const
{
	return ((point[0] + RAY_EPSILON >= min[0]) && (point[1] + RAY_EPSILON >= min[1]) && (point[2] + RAY_EPSILON >= min[2]) &&
		 (point[0] - RAY_EPSILON <= max[0]) && (point[1] - RAY_EPSILON <= max[1]) && (point[2] - RAY_EPSILON <= max[2]));
}

// if the ray hits the box, put the "t" value of the intersection
// closest to the origin in tMin and the "t" value of the far intersection
// in tMax and return true, else return false.
// Using Kay/Kajiya algorithm.
bool BoundingBox::intersect(const ray& r, double& tMin, double& tMax) const
{
	vec3f R0 = r.getPosition();
	vec3f Rd = r.getDirection();

	tMin = -1.0e308; // 1.0e308 is close to infinity... close enough for us!
	tMax = 1.0e308;
	double ttemp;
	
	for (int currentaxis = 0; currentaxis < 3; currentaxis++)
	{
		double vd = Rd[currentaxis];
		
		// if the ray is parallel to the face's plane (=0.0)
		if( vd == 0.0 )
			continue;

		double v1 = min[currentaxis] - R0[currentaxis];
		double v2 = max[currentaxis] - R0[currentaxis];

		// two slab intersections
		double t1 = v1/vd;
		double t2 = v2/vd;
		
		if ( t1 > t2 ) { // swap t1 & t2
			ttemp = t1;
			t1 = t2;
			t2 = ttemp;
		}

		if (t1 > tMin)
			tMin = t1;
		if (t2 < tMax)
			tMax = t2;

		if (tMin > tMax) // box is missed
			return false;
		if (tMax < 0.0) // box is behind ray
			return false;
	}
	return true; // it made it past all 3 axes.
}


bool Geometry::intersect(const ray&r, isect&i) const
{
    // Transform the ray into the object's local coordinate space
    vec3f pos = transform->globalToLocalCoords(r.getPosition());
    vec3f dir = transform->globalToLocalCoords(r.getPosition() + r.getDirection()) - pos;
    double length = dir.length();
    dir /= length;

    ray localRay( pos, dir );

    if (intersectLocal(localRay, i)) {
        // Transform the intersection point & normal returned back into global space.
		i.N = transform->localToGlobalCoordsNormal(i.N);
		i.t /= length;

		return true;
    } else {
        return false;
    }
    
}

bool Geometry::intersectLocal( const ray& r, isect& i ) const
{
	return false;
}

bool Geometry::hasBoundingBoxCapability() const
{
	// by default, primitives do not have to specify a bounding box.
	// If this method returns true for a primitive, then either the ComputeBoundingBox() or
    // the ComputeLocalBoundingBox() method must be implemented.

	// If no bounding box capability is supported for an object, that object will
	// be checked against every single ray drawn.  This should be avoided whenever possible,
	// but this possibility exists so that new primitives will not have to have bounding
	// boxes implemented for them.
	
	return false;
}

Scene::~Scene()
{
    giter g;
    liter l;
    
	for( g = objects.begin(); g != objects.end(); ++g ) {
		delete (*g);
	}

	for( g = boundedobjects.begin(); g != boundedobjects.end(); ++g ) {
		delete (*g);
	}

	for( g = nonboundedobjects.begin(); g != boundedobjects.end(); ++g ) {
		delete (*g);
	}

	for( l = lights.begin(); l != lights.end(); ++l ) {
		delete (*l);
	}

	if (m_ucTextureImage)
		delete[]m_ucTextureImage;
	if (m_ucNormalMap)
		delete[]m_ucNormalMap;
}

// Get any intersection with an object.  Return information about the 
// intersection through the reference parameter.
bool Scene::intersect( const ray& r, isect& i ) const
{
	typedef list<Geometry*>::const_iterator iter;
	iter j;

	isect cur;
	bool have_one = false;

	// try the non-bounded objects
	for( j = nonboundedobjects.begin(); j != nonboundedobjects.end(); ++j ) {
		if( (*j)->intersect( r, cur ) ) {
			if( !have_one || (cur.t < i.t) ) {
				i = cur;
				have_one = true;
			}
		}
	}

	// try the bounded objects
	for( j = boundedobjects.begin(); j != boundedobjects.end(); ++j ) {
		if( (*j)->intersect( r, cur ) ) {
			if( !have_one || (cur.t < i.t) ) {
				i = cur;
				have_one = true;
			}
		}
	}


	return have_one;
}

void Scene::initScene()
{
	bool first_boundedobject = true;
	BoundingBox b;
	
	typedef list<Geometry*>::const_iterator iter;
	// split the objects into two categories: bounded and non-bounded
	for( iter j = objects.begin(); j != objects.end(); ++j ) {
		if( (*j)->hasBoundingBoxCapability() )
		{
			boundedobjects.push_back(*j);

			// widen the scene's bounding box, if necessary
			if (first_boundedobject) {
				sceneBounds = (*j)->getBoundingBox();
				first_boundedobject = false;
			}
			else
			{
				b = (*j)->getBoundingBox();
				sceneBounds.max = maximum(sceneBounds.max, b.max);
				sceneBounds.min = minimum(sceneBounds.min, b.min);
			}
		}
		else
			nonboundedobjects.push_back(*j);
	}
}

void Scene::loadTextureImage(char* fn) {
	if (m_ucTextureImage)
		delete[]m_ucTextureImage;

	unsigned char*	data;
	int				width, height;

	if ((data = readBMP(fn, width, height)) == NULL)
	{
		fl_alert("Can't load bitmap file");
		return;
	}

	m_textureWidth = width;
	m_textureHeight = height;
	m_ucTextureImage = data;

	unsigned char* temp = filter();
	calcualteNormalMap(temp);//calculate the normal map of the loaded texture image
	delete[]temp;
}

vec3f Scene::getColor(double u, double v) {//given u , v from [0,1],return the corresponding color in of the texture map
	int x = u * m_textureWidth;
	if (x >= m_textureWidth)
		x = m_textureWidth - 1;
	int y = v * m_textureHeight;
	if (y >= m_textureHeight)
		y = m_textureHeight - 1;
	int index = (y * m_textureWidth + x) * 3;
	vec3f result;
	//cout << "(" << x << "," << y << ")";
	if(m_ucTextureImage)
		result = vec3f(m_ucTextureImage[index] / 255.0f, m_ucTextureImage[index + 1] / 255.0f, m_ucTextureImage[index + 2] / 255.0f);
	return result;
}

void Scene::calcualteNormalMap(unsigned char* filteredMap) {
	if (!filteredMap)
		return;

	if (!m_ucTextureImage)
		return;

	if (m_ucNormalMap)
		delete[]m_ucNormalMap;


	m_ucNormalMap = new unsigned char[m_textureWidth * m_textureHeight * 3];

	//cout << m_textureWidth * m_textureHeight * 3 << endl;

	unsigned char* greyScale = new unsigned char[m_textureHeight*m_textureWidth];
	for (int i = 0; i < m_textureHeight * m_textureWidth; i++)
		greyScale[i] = filteredMap[i * 3] * 0.299 + filteredMap[i * 3 + 1] * 0.587 + filteredMap[i * 3 + 2] * 0.114;
		//greyScale[i] = (filteredMap[i * 3] + filteredMap[i * 3 + 1] + filteredMap[i * 3 + 2]) / 3.0f;

	for (int Y = 0; Y < m_textureHeight; Y++) {
		for (int X = 0; X < m_textureWidth; X++) {
			int right;
			int up;
			int left;
			int down;

			//int current = greyScale[Y * m_textureWidth + X];

			if (X == m_textureWidth - 1)
				right = greyScale[Y * m_textureWidth + X];
			else
				right = greyScale[Y * m_textureWidth + X + 1];

			if (X == 0)
				left = greyScale[Y * m_textureWidth + X];
			else
				left = greyScale[Y * m_textureWidth + X - 1];

			if (Y == 0)
				down = greyScale[Y * m_textureWidth + X];
			else
				down = greyScale[(Y - 1) * m_textureWidth + X];

			if (Y == m_textureHeight - 1)
				up = greyScale[Y * m_textureWidth + X];
			else
				up = greyScale[(Y + 1) * m_textureWidth + X];


			vec3f a(1.0, 0.0, (right - left + 1) * 0.5);
			vec3f b(0.0, 1.0, (up - down + 1) * 0.5);
			vec3f n = (a ^ b).normalize();

			//float ha = (right - left + 1) * 0.5;
			//float hb = (up - down + 1) * 0.5;

			//cout << n[0] << "," << n[1] << "," << n[2] << endl;
			//cout << X << "," << Y << endl;
			
			m_ucNormalMap[(Y * m_textureWidth + X) * 3] = (n[0] + 1) * 0.5 * 255.0f;
			m_ucNormalMap[(Y * m_textureWidth + X) * 3 + 1] = (n[1] + 1) * 0.5 * 255.0f;
			m_ucNormalMap[(Y * m_textureWidth + X) * 3 + 2] = n[2] * 255.0f;
		}
	}
}

vec3f Scene::getTextureNormal(float x, float y) {
	int xx = x * m_textureWidth;
	if (xx >= m_textureWidth)
		xx = m_textureWidth - 1;
	int yy = y * m_textureHeight;
	if (yy >= m_textureHeight)
		yy = m_textureHeight - 1;

	int index = (yy * m_textureWidth + xx) * 3;
	return vec3f(m_ucNormalMap[index], m_ucNormalMap[index + 1], m_ucNormalMap[index + 2]);
}

void  Geometry::setTextureNormal(float x, float y,const mat4f& mat) {
	vec3f rgbNormal = scene->getTextureNormal(x, y);
	rgbNormal[0] = (rgbNormal[0] / 255.0f) * 2 - 1;
	rgbNormal[1] = (rgbNormal[1] / 255.0f) * 2 - 1;
	rgbNormal[2] = rgbNormal[2] / 255.0f;
	currentNormal = (mat * rgbNormal).normalize();
	//cout << "(" << currentNormal[0] << "," << currentNormal[1] << "," << currentNormal[2] << ")";
}

void Scene::saveImage(char *iname)
{

	if (m_ucNormalMap)
		writeBMP(iname, m_textureWidth, m_textureHeight, m_ucNormalMap);
}

unsigned char* Scene::filter() {
	if (!m_ucTextureImage)
		return NULL;

	unsigned char* temp = new unsigned char[m_textureWidth * m_textureHeight * 3];
	memset(temp, 0, m_textureWidth * m_textureHeight * 3);
	mat3f kernel(
		vec3f(0.0625, 0.125, 0.0625),
		vec3f(0.125, 0.25, 1.25),
		vec3f(0.0625, 0.125, 0.0625)
	);

	for (int Y = 0; Y < m_textureHeight; Y++) {
		for (int X = 0; X < m_textureWidth; X++) {
			int index2 = (Y * m_textureWidth + X) * 3;
			for (int j = -1; j < 2; j++) {
				for (int i = -1; i < 2; i++) {
					int x = abs(X + i);
					int y = abs(Y + j);
					if (x >= m_textureWidth)
						x = m_textureWidth - 2;
					if (y >= m_textureHeight)
						y = m_textureHeight - 2;

					int index = (y * m_textureWidth + x) * 3;
					temp[index2] += kernel[j + 1][i + 1] * m_ucTextureImage[index];
					temp[index2 + 1] += kernel[j + 1][i + 1] * m_ucTextureImage[index + 1];
					temp[index2 + 2] += kernel[j + 1][i + 1] * m_ucTextureImage[index + 2];
				}
			}
		}
	}

	return temp;
}

void Scene::loadNormalMap(char* fname) {
	if (m_ucNormalMap)
		delete[]m_ucNormalMap;

	unsigned char*	data;
	int				width, height;

	if (!m_ucTextureImage) {
		fl_alert("Load a texture image first!");
		return;
	}

	if ((data = readBMP(fname, width, height)) == NULL)
	{
		fl_alert("Can't load bitmap file");
		return;
	}

	if (width != m_textureWidth || height != m_textureHeight) {
		fl_alert("Size doesn`t match!");
		return;
	}

	m_ucNormalMap = data;
}

void Scene::loadHeightFieldMap(char* fname) {
	
	unsigned char* heightFieldImage;
	unsigned char* greyScaleMap;

	unsigned char*	data;
	int				width, height;


	if ((data = readBMP(fname, width, height)) == NULL)
	{
		fl_alert("Can't load bitmap file");
		return;
	}

	
	heightFieldImage = data;
	greyScaleMap = new unsigned char[width * height];

	for(int i = 0;i < width * height;i ++)
		greyScaleMap[i] = heightFieldImage[i * 3] * 0.299 + heightFieldImage[i * 3 + 1] * 0.587 + heightFieldImage[i * 3 + 2] * 0.114;

	Trimesh* mesh = new Trimesh(this,new Material,new TransformRoot);

	for (int Z = 0; Z < height; Z++) {
		for (int X = 0; X < width; X++) {
			Material* m = new Material;
			m->kd = vec3f(heightFieldImage[(Z * width + X) * 3] / 255.0f, heightFieldImage[(Z * width + X) * 3 + 1] / 255.0f, heightFieldImage[(Z * width + X) * 3 + 2] / 255.0f);
			float x = (float(X) / float(width)) * 2;
			float z = (float(Z) / float(height)) * 2;
			float y = float(greyScaleMap[Z * width + X]) / 255.0f;
			mesh->addVertex(vec3f(x, y, z));
			mesh->addMaterial(m);
			//cout << "(" << x << "," << y << "," << z << ")";
		}
	}
	int k = 0;
	for(int Y = 0; Y < height - 11; Y = Y + 10) {
		for (int X = 0; X < width - 11; X = X + 10) {
			mesh->addFace(Y * width + X, (Y + 10) * width + X, (Y + 10) * width + X + 10);
			mesh->addFace(Y * width + X, (Y + 10) * width + X + 1 + 10, Y * width + X + 10);
			//cout << k++ << "," << k++ << ",";
		}
	}
	boundedobjects.push_back(mesh);
	mesh->addToNBoundedObjects();
	//cout << "done" << endl;
	//cout << objects.size() << endl;
	//cout << boundedobjects.size();
	fl_message("done!");
}