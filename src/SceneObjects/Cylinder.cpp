#include <cmath>

#include "Cylinder.h"

bool Cylinder::intersectLocal( const ray& r, isect& i ) const
{
	i.obj = this;

	if( intersectCaps( r, i ) ) {
		isect ii;
		if( intersectBody( r, ii ) ) {
			if( ii.t < i.t ) {
				i = ii;
				i.obj = this;
			}
		}
		return true;
	} else {
		return intersectBody( r, i );
	}
}

bool Cylinder::intersectBody( const ray& r, isect& i ) const
{
	double x0 = r.getPosition()[0];
	double y0 = r.getPosition()[1];
	double x1 = r.getDirection()[0];
	double y1 = r.getDirection()[1];

	double a = x1*x1+y1*y1;
	double b = 2.0*(x0*x1 + y0*y1);
	double c = x0*x0 + y0*y0 - 1.0;

	if( 0.0 == a ) {
		// This implies that x1 = 0.0 and y1 = 0.0, which further
		// implies that the ray is aligned with the body of the cylinder,
		// so no intersection.
		return false;
	}

	double discriminant = b*b - 4.0*a*c;

	if( discriminant < 0.0 ) {
		return false;
	}
	
	discriminant = sqrt( discriminant );

	double t2 = (-b + discriminant) / (2.0 * a);

	if( t2 <= RAY_EPSILON ) {
		return false;
	}

	double t1 = (-b - discriminant) / (2.0 * a);

	if( t1 > RAY_EPSILON ) {
		// Two intersections.
		vec3f P = r.at( t1 );
		double z = P[2];
		if( z >= 0.0 && z <= 1.0 ) {
			// It's okay.
			i.t = t1;
			i.N = vec3f( P[0], P[1], 0.0 ).normalize();
			return true;
		}
	}

	vec3f P = r.at( t2 );
	double z = P[2];
	if( z >= 0.0 && z <= 1.0 ) {
		i.t = t2;

		vec3f normal( P[0], P[1], 0.0 );
		// In case we are _inside_ the _uncapped_ cone, we need to flip the normal.
		// Essentially, the cone in this case is a double-sided surface
		// and has _2_ normals
		if(normal.dot( r.getDirection() ) > 0 )
			normal = -normal;

		i.N = normal.normalize();
		return true;
	}

	return false;
}

bool Cylinder::intersectCaps( const ray& r, isect& i ) const
{
	if( !capped ) {
		return false;
	}

	double pz = r.getPosition()[2];
	double dz = r.getDirection()[2];

	if( 0.0 == dz ) {
		return false;
	}

	double t1;
	double t2;

	if( dz > 0.0 ) {
		t1 = (-pz)/dz;
		t2 = (1.0-pz)/dz;
	} else {
		t1 = (1.0-pz)/dz;
		t2 = (-pz)/dz;
	}

	if( t2 < RAY_EPSILON ) {
		return false;
	}

	if( t1 >= RAY_EPSILON ) {
		vec3f p( r.at( t1 ) );
		if( (p[0]*p[0] + p[1]*p[1]) <= 1.0 ) {
			i.t = t1;
			if( dz > 0.0 ) {
				// Intersection with cap at z = 0.
				i.N = vec3f( 0.0, 0.0, -1.0 );
			} else {
				i.N = vec3f( 0.0, 0.0, 1.0 );
			}
			return true;
		}
	}

	vec3f p( r.at( t2 ) );
	if( (p[0]*p[0] + p[1]*p[1]) <= 1.0 ) {
		i.t = t2;
		if( dz > 0.0 ) {
			// Intersection with cap at z = 1.
			i.N = vec3f( 0.0, 0.0, 1.0 );
		} else {
			i.N = vec3f( 0.0, 0.0, -1.0 );
		}
		return true;
	}

	return false;
}

T Cylinder::getPrimitiveT(const ray& r) {
	vec3f pos = transform->globalToLocalCoords(r.getPosition());
	vec3f dir = (transform->globalToLocalCoords(r.getPosition() + r.getDirection()) - pos);
	float length = dir.length();
	dir = dir.normalize();
	ray localRay(pos, dir);

	T body = intersectBody(localRay);
	T caps = intersectCaps(localRay);
	body[0] /= length; body[1] /= length;
	caps[0] /= length; caps[1] /= length;
	if (body[0] == -1 && caps[0] == -1)
		return T();
	else if (body[0] > RAY_EPSILON && body[1] > RAY_EPSILON)
		return body;
	else if (caps[0] > RAY_EPSILON && caps[1] > RAY_EPSILON)
		return caps;
	else if (caps[0] > RAY_EPSILON && caps[1] < RAY_EPSILON) {
		//cout <<"caps[0]:"<< caps[0] << ",body[1]:" << body[1] << endl;
		return T(caps[0], body[1]);
	}
	else if(body[0] > RAY_EPSILON && body[1] < RAY_EPSILON){
		//cout << "body[0]:" << body[0] << ",caps[1]:" << caps[1] << endl;
		return T(body[0], caps[1]);
	}
	else {
		return T();
	}
}

T Cylinder::intersectBody(const ray& r)const {
	double x0 = r.getPosition()[0];
	double y0 = r.getPosition()[1];
	double x1 = r.getDirection()[0];
	double y1 = r.getDirection()[1];

	double a = x1*x1 + y1*y1;
	double b = 2.0*(x0*x1 + y0*y1);
	double c = x0*x0 + y0*y0 - 1.0;

	if (0.0 == a) {
		return T(-1, -1);
	}

	double discriminant = b*b - 4.0*a*c;

	if (discriminant < 0.0) {
		return T(-1, -1);
	}

	discriminant = sqrt(discriminant);

	double t2 = (-b + discriminant) / (2.0 * a);

	if (t2 <= RAY_EPSILON) {
		return T(-1, -1);
	}

	double t1 = (-b - discriminant) / (2.0 * a);

	if (t1 > RAY_EPSILON) {
		vec3f P = r.at(t1);
		double z = P[2];
		if (!(z >= 0.0 && z <= 1.0)) {
			t1 = -1;
		}
	}

	vec3f P = r.at(t2);
	double z = P[2];
	if (!(z >= 0.0 && z <= 1.0)) {
		t2 = -1;
	}

	return T(t1, t2);
}

T Cylinder::intersectCaps(const ray& r)const {
	if (!capped) {
		return T(-1, -1);
	}

	double pz = r.getPosition()[2];
	double dz = r.getDirection()[2];

	if (0.0 == dz) {
		return  T(-1, -1);
	}

	double t1;
	double t2;

	if (dz > 0.0) {
		t1 = (-pz) / dz;
		t2 = (1.0 - pz) / dz;
	}
	else {
		t1 = (1.0 - pz) / dz;
		t2 = (-pz) / dz;
	}

	if (t2 < RAY_EPSILON) {
		return  T(-1, -1);
	}


	vec3f p(r.at(t1));
	if ((p[0] * p[0] + p[1] * p[1]) > 1.0) {
		t1 = -1;
	}
	

	p = r.at(t2);
	if ((p[0] * p[0] + p[1] * p[1]) >1.0) {
		if (t1 == -1)
			return T(-1, -1);
		t2 = -1;
	}

	return T(t1, t2);
}
