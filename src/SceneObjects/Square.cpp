#include <cmath>

#include "Square.h"

bool Square::intersectLocal( const ray& r, isect& i ) const
{
	vec3f p = r.getPosition();
	vec3f d = r.getDirection();

	if( d[2] == 0.0 ) {
		return false;
	}

	double t = -p[2]/d[2];

	if( t <= RAY_EPSILON ) {
		return false;
	}

	vec3f P = r.at( t );

	if( P[0] < -0.5 || P[0] > 0.5 ) {	
		return false;
	}

	if( P[1] < -0.5 || P[1] > 0.5 ) {	
		return false;
	}

	i.obj = this;
	i.t = t;
	if( d[2] > 0.0 ) {
		i.N = vec3f( 0.0, 0.0, -1.0 );
	} else {
		i.N = vec3f( 0.0, 0.0, 1.0 );
	}

	return true;
}

vec3f Square::getTextureColor( vec3f& intersecPoint) {
	if (!scene->isTextureLoaded())
		return vec3f(0.0, 0.0, 0.0);
	vec3f localPoint = transform->globalToLocalCoords(intersecPoint);
	double u = localPoint[0] + 0.5;
	double v = localPoint[1] + 0.5;

	//cout << "(" << u << "," << v << ")";
	mat4f a;
	
	setTextureNormal(u, v, a.identity());
	return scene->getColor(u, v);
}