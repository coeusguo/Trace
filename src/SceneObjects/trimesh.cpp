#include <cmath>
#include <float.h>
#include "trimesh.h"

Trimesh::~Trimesh()
{
    for( Materials::iterator i = materials.begin(); i != materials.end(); ++i )
    {
        delete *i;
    }
}

// must add vertices, normals, and materials IN ORDER
void Trimesh::addVertex( const vec3f &v )
{
    vertices.push_back( v );
}

void Trimesh::addMaterial( Material *m )
{
    materials.push_back( m );
}

void Trimesh::addNormal( const vec3f &n )
{
    normals.push_back( n );
}

// Returns false if the vertices a,b,c don't all exist
bool Trimesh::addFace( int a, int b, int c )
{
    int vcnt = vertices.size();

    if( a >= vcnt || b >= vcnt || c >= vcnt )
        return false;

    TrimeshFace *newFace = new TrimeshFace( scene, new Material(*this->material), this, a, b, c );
    newFace->setTransform(this->transform);
    faces.push_back( newFace );
    scene->add(newFace);
    return true;
}

char *
Trimesh::doubleCheck()
// Check to make sure that if we have per-vertex materials or normals
// they are the right number.
{
    if( materials.size() && materials.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of materials.";
    if( normals.size() && normals.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of normals.";

    return 0;
}

// Intersect ray r with the triangle abc.  If it hits returns true,
// and put the parameter in t and the barycentric coordinates of the
// intersection in bary.
// Uses the algorithm and notation from _Graphic Gems 5_, p. 232.
//
// Calculates and returns the normal of the triangle too.
bool TrimeshFace::intersectLocal( const ray& r, isect& i ) const
{
    const vec3f& a = parent->vertices[ids[0]];
    const vec3f& b = parent->vertices[ids[1]];
    const vec3f& c = parent->vertices[ids[2]];
    
    vec3f bary;
    float t;
    vec3f n;
    
    vec3f p = r.getPosition();
    vec3f v = r.getDirection();
    
    vec3f ab = b - a;
    vec3f ac = c - a;
    vec3f ap = p - a;
    
	vec3f cv=ab.cross(ac);

	// there exists some bad triangles such that two vertices coincide
	// check this before normalize
	if (cv.iszero()) return false;
    n = (cv).normalize();
	
    double vdotn = v*n;
    if( -vdotn < NORMAL_EPSILON )
        return false;
    
    t = - (ap*n)/vdotn;
    
    if( t < RAY_EPSILON )
        return false;

    // find k where k is the index of the component
    // of normal vector with greatest absolute value
    float greatestMag = FLT_MIN;
    int k = -1;
    for( int j = 0; j < 3; ++j )
    {
        float val = n[j];
        if( val < 0 )
            val *= -1;
        if( val > greatestMag )
        {
            k = j;
            greatestMag = val;
        }
    }

    vec3f am = ap + t * v;
    
	bary[1] = (am.cross(ac))[k]/(ab.cross(ac))[k];
    bary[2] = (ab.cross(am))[k]/(ab.cross(ac))[k];
    bary[0] = 1-bary[1]-bary[2];
    if( bary[0] < 0 || bary[1] < 0 || bary[1] > 1 || bary[2] < 0 || bary[2] > 1 )
        return false;

    // if we get this far, we have an intersection.  Fill in the info.
    i.setT( t );
    if(parent->normals.size())
    {
        // use interpolated normals
        i.setN( (bary[0] * parent->normals[ids[0]]
                 + bary[1] * parent->normals[ids[1]]
                 + bary[2] * parent->normals[ids[2]]).normalize() );
    } else {
        i.setN( n );           // use face normal
    }
    i.obj = this;

    // linearly interpolate materials
    if( parent->materials.size() )
    {
        Material *m = new Material();
        for( int jj = 0; jj < 3; ++jj )
            (*m) += bary[jj] * (*parent->materials[ ids[jj] ]);
        i.setMaterial( m );
    }
    
    return true;
}

void
Trimesh::generateNormals()
// Once you've loaded all the verts and faces, we can generate per
// vertex normals by averaging the normals of the neighboring faces.
{
    int cnt = vertices.size();
    normals.resize( cnt );
    int *numFaces = new int[ cnt ]; // the number of faces assoc. with each vertex
    memset( numFaces, 0, sizeof(int)*cnt );
    
    for( Faces::iterator fi = faces.begin(); fi != faces.end(); ++fi )
    {
        vec3f a = vertices[(**fi)[0]];
        vec3f b = vertices[(**fi)[1]];
        vec3f c = vertices[(**fi)[2]];
        
        vec3f faceNormal = ((b-a).cross(c-a)).normalize();
        
        for( int i = 0; i < 3; ++i )
        {
            normals[(**fi)[i]] += faceNormal;
            ++numFaces[(**fi)[i]];
        }
    }

    for( int i = 0; i < cnt; ++i )
    {
        if( numFaces[i] )
            normals[i]  /= numFaces[i];
    }

    delete [] numFaces;
}

