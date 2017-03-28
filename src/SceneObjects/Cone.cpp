
#include <cmath>

#include "Cone.h"

bool Cone::intersectLocal( const ray& r, isect& i ) const
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


bool Cone::intersectBody( const ray& r, isect& i ) const
{
	vec3f d = r.getDirection();
	vec3f p = r.getPosition();

	double a = (d[0]*d[0]) + (d[1]*d[1]) - (C*d[2]*d[2]);
	double b = 2.0 * (d[0]*p[0] + d[1]*p[1] - C*d[2]*p[2]) - B*d[2];
	double c = (p[0]*p[0]) + (p[1]*p[1]) - A - (B*p[2]) - (C*p[2]*p[2]);

	double disc = b*b - 4.0*a*c;

	if( disc <= 0.0 ) {
		return false;
	}

	disc = sqrt( disc );

	double t1 = (-b - disc) / (2.0 * a);
	double t2 = (-b + disc) / (2.0 * a);

	if( t2 < RAY_EPSILON ) {
		return false;
	}

	if( t1 > RAY_EPSILON ) {
		// Two intersections.
		vec3f P = r.at( t1 );
		double z = P[2];
		if( z >= 0.0 && z <= height ) {
			// It's okay.
			i.t = t1;
            i.N = vec3f( P[0], P[1], 
              -(C*P[2]+(t_radius-b_radius)*t_radius/height)).normalize();
				
			
			return true;
		}
	}

	vec3f P = r.at( t2 );
	double z = P[2];
	if( z >= 0.0 && z <= height ) {
		i.t = t2;
        i.N = vec3f( P[0], P[1], 
              -(C*P[2]+(t_radius-b_radius)*t_radius/height)).normalize();
		// In case we are _inside_ the _uncapped_ cone, we need to flip the normal.
		// Essentially, the cone in this case is a double-sided surface
		// and has _2_ normals
	
		if( !capped && (i.N).dot( r.getDirection() ) > 0 )
				i.N = -i.N;

        return true;
	}

	return false;
}

bool Cone::intersectCaps( const ray& r, isect& i ) const
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
	double r1;
	double r2;

	if( dz > 0.0 ) {
		t1 = (-pz)/dz;
		t2 = (height-pz)/dz;
		r1 = b_radius;
		r2 = t_radius;
	} else {
		t1 = (height-pz)/dz;
		t2 = (-pz)/dz;
		r1 = t_radius;
		r2 = b_radius;
	}

	if( t2 < RAY_EPSILON ) {
		return false;
	}

	if( t1 >= RAY_EPSILON ) {
		vec3f p( r.at( t1 ) );
		if( (p[0]*p[0] + p[1]*p[1]) <= r1 * r1 ) {
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
	if( (p[0]*p[0] + p[1]*p[1]) <= r2 * r2 ) {
		i.t = t2;
		if( dz > 0.0 ) {
			// Intersection with interior of cap at z = 1.
			i.N = vec3f( 0.0, 0.0, 1.0 );
		} else {
			i.N = vec3f( 0.0, 0.0, -1.0 );
		}
		return true;
	}

	return false;
}
