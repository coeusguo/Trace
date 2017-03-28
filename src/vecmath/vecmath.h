#ifndef __VECMATH_H__
#define __VECMATH_H__

// Vector math classes and support routines.
// This was taken out of someone's algebra code from the 457 devl directory.

#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

class vec3f;
class vec4f;
class mat3f;
class mat4f;

// used as an exception during matrix inversion.
class SingularMatrixException
{};

inline double minimum( double a, double b )
{
	return a < b ? a : b;
}

inline double maximum( double a, double b )
{
	return a > b ? a : b;
}

class vec3f
{
public:
	// Constructors

	vec3f() { n[0] = 0.0; n[1] = 0.0; n[2] = 0.0; }
	vec3f( const double x, const double y, const double z )
		{ n[0] = x; n[1] = y; n[2] = z; }
//	vec3f( const double d )
//		{ n[0] = d; n[1] = d; n[2] = d; }
	vec3f( const vec3f& v )
		{ n[0] = v.n[0]; n[1] = v.n[1]; n[2] = v.n[2]; }
	vec3f( const vec4f& v4 );

	vec3f& operator	=( const vec3f& v )
		{ n[0] = v.n[0]; n[1] = v.n[1]; n[2] = v.n[2]; return *this; }
	vec3f& operator +=( const vec3f& v )
		{ n[0] += v.n[0]; n[1] += v.n[1]; n[2] += v.n[2]; return *this; }
	vec3f& operator -= ( const vec3f& v )
		{ n[0] -= v.n[0]; n[1] -= v.n[1]; n[2] -= v.n[2]; return *this; }
	vec3f& operator *= ( const double d )
		{ n[0] *= d; n[1] *= d; n[2] *= d; return *this; }
	vec3f& operator /= ( const double d )
		{ n[0] /= d; n[1] /= d; n[2] /= d; return *this; }

	double& operator []( int i )
		{ return n[i]; }
	double operator []( int i ) const 
		{ return n[i]; }

	// Cross product between this and 'b'
	vec3f cross(const vec3f& b) const
	{
		return vec3f(
			n[1]*b.n[2] - n[2]*b.n[1],
			n[2]*b.n[0] - n[0]*b.n[2],
			n[0]*b.n[1] - n[1]*b.n[0] );
	}

	// Clamps each component to the range 0.0 <= n <= 1.0
	vec3f clamp() const
	{
		vec3f a;
	
		a[0] = maximum(0.0, minimum(n[0], 1.0));
		a[1] = maximum(0.0, minimum(n[1], 1.0));
		a[2] = maximum(0.0, minimum(n[2], 1.0));

		return a;
	}

	// Dot product of this and 'b'
	double dot(const vec3f& b) const
	{
		return n[0]*b[0] + n[1]*b[1] + n[2]*b[2];
	}

	double length_squared() const
		{ return n[0]*n[0] + n[1]*n[1] + n[2]*n[2]; }
	double length() const
		{ return sqrt( length_squared() ); }
	vec3f normalize() const
	{ 
		vec3f ret( *this );
		ret /= length();
		return ret;
	}

	bool iszero() const { return ( (n[0]==0 && n[1]==0 && n[2]==0) ? true : false); };

public:
	double n[3];
};

class vec4f
{
public:
	// Constructors

	vec4f() { n[0] = 0.0; n[1] = 0.0; n[2] = 0.0; n[3] = 0.0; }
	vec4f( const double x, const double y, const double z, const double w )
		{ n[0] = x; n[1] = y; n[2] = z; n[3] = w; }
//	vec4f( const double d )
//		{ n[0] = d; n[1] = d; n[2] = d; n[3] = d; }
	vec4f( const vec4f& v )
		{ n[0] = v.n[0]; n[1] = v.n[1]; n[2] = v.n[2]; n[3] = v.n[3]; }
	vec4f( const vec3f& v )
		{ n[0] = v[0]; n[1] = v[1]; n[2] = v[2]; n[3] = 1.0; }

	vec4f& operator =( const vec4f& v )
		{ n[0] = v.n[0]; n[1] = v.n[1]; n[2] = v.n[2]; n[3] = v.n[3];
		  return *this; }
	vec4f& operator +=( const vec4f& v )
		{ n[0] += v.n[0]; n[1] += v.n[1]; n[2] += v.n[2]; n[3] += v.n[3];
		  return *this; }
	vec4f& operator -= ( const vec4f& v )
		{ n[0] -= v.n[0]; n[1] -= v.n[1]; n[2] -= v.n[2]; n[3] -= v.n[3];
		  return *this; }
	vec4f& operator *= ( const double d )
		{ n[0] *= d; n[1] *= d; n[2] *= d; n[3] *= d; return *this; }
	vec4f& operator /= ( const double d )
		{ n[0] /= d; n[1] /= d; n[2] /= d; n[3] /= d; return *this; }
	double& operator []( int i )
		{ return n[i]; }
	double operator []( int i ) const 
		{ return n[i]; }

	// Dot product of this and 'b'
	double dot(const vec4f& b) const
	{
		return n[0]*b[0] + n[1]*b[1] + n[2]*b[2] + n[3]*b[3];
	}

	// Clamps each component to the range 0.0 <= n <= 1.0
	vec4f clamp() const
	{
		vec4f a;
	
		a[0] = maximum(0.0, minimum(n[0], 1.0));
		a[1] = maximum(0.0, minimum(n[1], 1.0));
		a[2] = maximum(0.0, minimum(n[2], 1.0));
		a[3] = maximum(0.0, minimum(n[3], 1.0));

		return a;
	}


	double length_squared() const
		{ return n[0]*n[0] + n[1]*n[1] + n[2]*n[2] + n[3]*n[3]; }
	double length() const
		{ return sqrt( length_squared() ); }
	vec4f normalize() const
		// { return *this / length(); }
	{ 
		vec4f ret( *this );
		ret /= length();
		return ret;
	}

public:
	double n[4];
};

class mat3f
{
public:
	mat3f()
		{ v[0] = vec3f(); v[1] = vec3f(); v[2] = vec3f();
		  v[0][0] = 1.0; v[1][1] = 1.0; v[2][2] = 1.0; }
	mat3f( const vec3f& v0, const vec3f& v1, const vec3f& v2 )
		{ v[0] = v0; v[1] = v1; v[2] = v2; }
//	mat3f( const double d )
//		{ v[0] = vec3f(); v[1] = vec3f(); v[2] = vec3f();
//		  v[0][0] = d; v[1][1] = d; v[2][2] = d; }
	mat3f( const mat3f& m )
		{ v[0] = m.v[0]; v[1] = m.v[1]; v[2] = m.v[2]; }

	mat3f& operator =( const mat3f& m )
		{ v[0] = m.v[0]; v[1] = m.v[1]; v[2] = m.v[2]; return *this; }
	mat3f& operator +=( const mat3f& m )
		{ v[0] += m.v[0]; v[1] += m.v[1]; v[2] += m.v[2]; return *this; }
	mat3f& operator -=( const mat3f& m )
		{ v[0] -= m.v[0]; v[1] -= m.v[1]; v[2] -= m.v[2]; return *this; }
	mat3f& operator *=( const double d )
		{ v[0] *= d; v[1] *= d; v[2] *= d; return *this; }
	mat3f& operator /=( const double d )
		{ v[0] /= d; v[1] /= d; v[2] /= d; return *this; }

	vec3f& operator []( int i )
		{ return v[i]; }
	const vec3f& operator []( int i ) const
		{ return v[i]; }
	
	vec3f column( int i ) const
		{ return vec3f( v[0][i], v[1][i], v[2][i] ); }

	// special functions

	mat3f transpose() const
	{
		return mat3f( column( 0 ), column( 1 ), column( 2 ) );
	}

	mat3f inverse() const;

public:
	vec3f v[3];
};

class mat4f
{
public:
	mat4f()
		{ v[0]=vec4f(); v[1]=vec4f(); v[2]=vec4f(); v[3]=vec4f();
		  v[0][0]=1.0; v[1][1]=1.0; v[2][2]=1.0; v[3][3]=1.0; }
	mat4f( const vec4f& v0, const vec4f& v1, const vec4f& v2, const vec4f& v3 )
		{ v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3; }
//	mat4f( const double d )
//		{ v[0]=vec4f(); v[1]=vec4f(); v[2]=vec4f(); v[3]=vec4f();
//		  v[0][0]=d; v[1][1]=d; v[2][2]=d; v[3][3]=d; }
	mat4f( const mat4f& m )
		{ v[0] = m.v[0]; v[1] = m.v[1]; v[2] = m.v[2]; v[3] = m.v[3]; }

	mat4f& operator =( const mat4f& m )
		{ v[0] = m.v[0]; v[1] = m.v[1]; v[2] = m.v[2]; v[3] = m.v[3];
		  return *this; }
	mat4f& operator +=( const mat4f& m )
		{ v[0] += m.v[0]; v[1] += m.v[1]; v[2] += m.v[2]; v[3] += m.v[3];
		  return *this; }
	mat4f& operator -=( const mat4f& m )
		{ v[0] -= m.v[0]; v[1] -= m.v[1]; v[2] -= m.v[2]; v[3] -= m.v[3];
		  return *this; }
	mat4f& operator *=( const double d )
		{ v[0] *= d; v[1] *= d; v[2] *= d; v[3] *= d; return *this; }
	mat4f& operator /=( const double d )
		{ v[0] /= d; v[1] /= d; v[2] /= d; v[3] /= d; return *this; }

	vec4f& operator []( int i )
		{ return v[i]; }
	const vec4f& operator []( int i ) const
		{ return v[i]; }
	vec4f column( int i ) const
		{ return vec4f( v[0][i], v[1][i], v[2][i], v[3][i] ); }

	mat4f transpose() const
		{ return mat4f( column( 0 ), column( 1 ), column( 2 ), column( 3 ) ); }
	mat4f inverse() const;
	mat3f upper33() const
		{ return mat3f( vec3f( v[0] ), vec3f( v[1] ), vec3f( v[2] ) ); }

	static mat4f identity()
	{ return mat4f(
		vec4f( 1.0, 0.0, 0.0, 0.0 ),
		vec4f( 0.0, 1.0, 0.0, 0.0 ),
		vec4f( 0.0, 0.0, 1.0, 0.0 ),
		vec4f( 0.0, 0.0, 0.0, 1.0 )); }

	static mat4f translate( const vec3f& v )
	{ return mat4f(
		vec4f( 1.0, 0.0, 0.0, v[0] ),
		vec4f( 0.0, 1.0, 0.0, v[1] ),
		vec4f( 0.0, 0.0, 1.0, v[2] ),
		vec4f( 0.0, 0.0, 0.0, 1.0 )); }

	static mat4f rotate( const vec3f& axis, const double angle ) { 
		double c = cos( angle );
		double s = sin( angle );
		double t = 1.0 - c;

		vec3f a = axis.normalize();
		return mat4f(
			vec4f(t*a[0]*a[0]+c, t*a[0]*a[1]-s*a[2], t*a[0]*a[2]+s*a[1], 0.0),
			vec4f(t*a[0]*a[1]+s*a[2], t*a[1]*a[1]+c, t*a[1]*a[2]-s*a[0], 0.0),
			vec4f(t*a[0]*a[2]-s*a[1], t*a[1]*a[2]+s*a[0], t*a[2]*a[2]+c, 0.0),
			vec4f(0.0, 0.0, 0.0, 1.0) );
	}

	static mat4f scale( const vec3f& t )
	{ return mat4f(
		vec4f( t[0], 0.0, 0.0, 0.0 ),
		vec4f( 0.0, t[1], 0.0, 0.0 ),
		vec4f( 0.0, 0.0, t[2], 0.0 ),
		vec4f( 0.0, 0.0, 0.0, 1.0 )); }

	static mat4f perspective3D( const double d )
	{ return mat4f(
		vec4f( 1.0, 0.0, 0.0, 0.0 ),
		vec4f( 0.0, 1.0, 0.0, 0.0 ),
		vec4f( 0.0, 0.0, 1.0, 0.0 ),
		vec4f( 0.0, 0.0, 1.0/d, 0.0 )); }

public:
	vec4f v[4];
};

/****************************************************************
*								*
*	       2D functions and 3D functions			*
*								*
****************************************************************/

mat3f identity2D();					    // identity 2D
mat4f identity3D();					    // identity 3D
mat4f translation3D(vec3f& v);				    // translation 3D
mat4f rotation3D(vec3f& Axis, const double angleDeg);	    // rotation 3D
mat4f scaling3D(vec3f& scaleVector);			    // scaling 3D
mat4f perspective3D(const double d);			    // perspective 3D

// And now, many inline functions are defined.

inline double operator *( const vec3f& a, const vec4f& b )
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + b[3];
}

inline double operator *( const vec4f& b, const vec3f& a )
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + b[3];
}

inline vec3f operator -(const vec3f& v)
{
	return vec3f( -v.n[0], -v.n[1], -v.n[2] );
}

inline vec3f operator +(const vec3f& a, const vec3f& b)
{
	return vec3f( a.n[0] + b.n[0], a.n[1] + b.n[1], a.n[2] + b.n[2] );
}

inline vec3f operator -(const vec3f& a, const vec3f& b)
{
	return vec3f( a.n[0] - b.n[0], a.n[1] - b.n[1], a.n[2] - b.n[2] );
}

inline vec3f operator *(const vec3f& a, const double d )
{
	return vec3f( a.n[0] * d, a.n[1] * d, a.n[2] * d );
}

inline vec3f operator *(const double d, const vec3f& a)
{
	return a * d;
}

inline vec3f operator *(const mat4f& a, const vec3f& v)
{
	return vec3f( a[0] * v, a[1] * v, a[2] * v ); 
}

inline vec3f operator *(const vec3f& v, mat4f& a)
{
	return a.transpose() * v;
}

inline double operator *(const vec3f& a, const vec3f& b)
{
	return a.n[0]*b.n[0] + a.n[1]*b.n[1] + a.n[2]*b.n[2];
}

inline vec3f operator *( const mat3f& a, const vec3f& b )
{
	return vec3f( a[0]*b, a[1]*b, a[2]*b );
}

inline vec3f operator *( const vec3f& a, const mat3f& b )
{
	return vec3f( b.column(0)*a, b.column(1)*a, b.column(2)*a );
}

inline vec3f operator /(const vec3f& a, const double d)
{
	return vec3f( a.n[0] / d, a.n[1] / d, a.n[2] / d );
}

/* // the vector cross product
inline vec3f operator ^(const vec3f& a, const vec3f& b)
{
	return vec3f(
		a.n[1]*b.n[2] - a.n[2]*b.n[1],
		a.n[2]*b.n[0] - a.n[0]*b.n[2],
		a.n[0]*b.n[1] - a.n[1]*b.n[0] );
}
*/

inline bool operator ==(const vec3f& a, const vec3f& b)
{
	return a.n[0]==b.n[0] && a.n[1] == b.n[1] && a.n[2] == b.n[2];
}

inline bool operator !=(const vec3f& a, const vec3f& b)
{
	return !( a == b );
}

inline ostream& operator <<( ostream& os, const vec3f& v )
{
	return os << v.n[0] << " " << v.n[1] << " " << v.n[2];
}

inline istream& operator >>( istream& is, vec3f& v )
{
	return is >> v.n[0] >> v.n[1] >> v.n[2];
}

inline void swap( vec3f& a, vec3f& b )
{
	vec3f t( a );
	a = b;
	b = t;
}

inline vec3f minimum( const vec3f& a, const vec3f& b )
{
	return vec3f( minimum(a.n[0],b.n[0]), minimum(a.n[1],b.n[1]), minimum(a.n[2],b.n[2]) );
}

inline vec3f maximum(const vec3f& a, const vec3f& b)
{
	return vec3f( maximum(a.n[0],b.n[0]), maximum(a.n[1],b.n[1]), maximum(a.n[2],b.n[2]) );
}

inline vec3f prod(const vec3f& a, const vec3f& b )
{
	return vec3f( a.n[0]*b.n[0], a.n[1]*b.n[1], a.n[2]*b.n[2] );
}

inline vec4f operator -( const vec4f& v )
{
	return vec4f( -v.n[0], -v.n[1], -v.n[2], -v.n[3] );
}

inline vec4f operator +( const vec4f& a, const vec4f& b )
{
	return vec4f( a.n[0] + b.n[0], a.n[1] + b.n[1], a.n[2] + b.n[2],
		a.n[3] + b.n[3] );
}

inline vec4f operator -(const vec4f& a, const vec4f& b)
{
	return vec4f( a.n[0] - b.n[0], a.n[1] - b.n[1], a.n[2] - b.n[2],
		a.n[3] - b.n[3] );
}

inline vec4f operator *(const vec4f& a, const double d )
{
	return vec4f( a.n[0] * d, a.n[1] * d, a.n[2] * d, a.n[3] * d );
}

inline vec4f operator *(const double d, const vec4f& a)
{
	return a * d;
}

inline double operator *(const vec4f& a, const vec4f& b)
{
	return a.n[0]*b.n[0] + a.n[1]*b.n[1] + a.n[2]*b.n[2] + a.n[3]*b.n[3];
}

inline vec4f operator *(const mat4f& a, const vec4f& v)
{
	return vec4f( a[0] * v, a[1] * v, a[2] * v, a[3] * v );
}

inline vec4f operator *( const vec4f& v, mat4f& a )
{
	return a.transpose() * v;
}

inline vec4f operator /(const vec4f& a, const double d)
{
	return vec4f( a.n[0] / d, a.n[1] / d, a.n[2] / d, a.n[3] / d );
}

inline bool operator ==(const vec4f& a, const vec4f& b)
{
	return a.n[0] == b.n[0] && a.n[1] == b.n[1] && a.n[2] == b.n[2] 
	    && a.n[3] == b.n[3];
}

inline bool operator !=(const vec4f& a, const vec4f& b)
{
	return !( a == b );
}

inline ostream& operator <<( ostream& os, const vec4f& v )
{
	return os << v.n[0] << " " << v.n[1] << " " << v.n[2] << " " << v.n[3];
}

inline istream& operator >>( istream& is, vec4f& v )
{
	return is >> v.n[0] >> v.n[1] >> v.n[2] >> v.n[3];
}

inline void swap( vec4f& a, vec4f& b )
{
	vec4f t( a );
	a = b;
	b = t;
}

inline vec4f minimum( const vec4f& a, const vec4f& b )
{
	return vec4f( minimum(a.n[0],b.n[0]), minimum(a.n[1],b.n[1]), minimum(a.n[2],b.n[2]),
	             minimum(a.n[3],b.n[3]) );
}

inline vec4f maximum(const vec4f& a, const vec4f& b)
{
	return vec4f( maximum(a.n[0],b.n[0]), maximum(a.n[1],b.n[1]), maximum(a.n[2],b.n[2]),
	             maximum(a.n[3],b.n[3]) );
}

inline vec4f prod(const vec4f& a, const vec4f& b )
{
	return vec4f( a.n[0]*b.n[0], a.n[1]*b.n[1], a.n[2]*b.n[2], a.n[3]*b.n[3] );
}

inline mat3f operator -( const mat3f& a )
{
	return mat3f( -a.v[0], -a.v[1], -a.v[2] );
}

inline mat3f operator +( const mat3f& a, const mat3f& b )
{
	return mat3f( a.v[0]+b.v[0], a.v[1]+b.v[1], a.v[2]+b.v[2] );
}

inline mat3f operator -( const mat3f& a, const mat3f& b)
{
	return mat3f( a.v[0]-b.v[0], a.v[1]-b.v[1], a.v[2]-b.v[2] );
}

inline mat3f operator *( const mat3f& a, const mat3f& b )
{
	vec3f c0 = b.column( 0 );
	vec3f c1 = b.column( 1 );
	vec3f c2 = b.column( 2 );

	return mat3f( 
		vec3f( a.v[0]*c0, a.v[0]*c1, a.v[0]*c2 ),
		vec3f( a.v[1]*c0, a.v[1]*c1, a.v[1]*c2 ),
		vec3f( a.v[2]*c0, a.v[2]*c1, a.v[2]*c2 ) );
}

inline mat3f operator *( const mat3f& a, const double d )
{
	return mat3f( a.v[0]*d, a.v[1]*d, a.v[2]*d );
}

inline mat3f operator *( const double d, const mat3f& a )
{
	return mat3f( d*a.v[0], d*a.v[1], d*a.v[2] );
}

inline mat3f operator /( const mat3f& a, const double d )
{
	return mat3f( a.v[0]/d, a.v[1]/d, a.v[2]/d );
}

inline bool operator ==( const mat3f& a, const mat3f& b )
{
	return a.v[0]==b.v[0] && a.v[1]==b.v[1] && a.v[2]==b.v[2];
}

inline bool operator !=( const mat3f& a, const mat3f& b )
{
	return !( a == b );
}

inline ostream& operator <<( ostream& os, const mat3f& m )
{
	os << m.v[0] << " " << m.v[1] << " " << m.v[2];
}

inline istream& operator >>( istream& is, mat3f& m )
{
	is >> m.v[0] >> m.v[1] >> m.v[2];
}

inline void swap(mat3f& a, mat3f& b)
{
	swap( a.v[0], b.v[0] );
	swap( a.v[1], b.v[1] );
	swap( a.v[2], b.v[2] );
}

inline mat4f operator -( const mat4f& a )
{
	return mat4f( -a.v[0], -a.v[1], -a.v[2], -a.v[3] );
}

inline mat4f operator +( const mat4f& a, const mat4f& b )
{
	return mat4f( a.v[0]+b.v[0], a.v[1]+b.v[1], a.v[2]+b.v[2], a.v[3]+b.v[3] );
}

inline mat4f operator -( const mat4f& a, const mat4f& b )
{
	return mat4f( a.v[0]-b.v[0], a.v[1]-b.v[1], a.v[2]-b.v[2], a.v[3]-b.v[3] );
}

inline mat4f operator *( const mat4f& a, const mat4f& b )
{
	vec4f c0 = b.column( 0 );
	vec4f c1 = b.column( 1 );
	vec4f c2 = b.column( 2 );
	vec4f c3 = b.column( 3 );

	return mat4f( 
		vec4f( a.v[0]*c0, a.v[0]*c1, a.v[0]*c2, a.v[0]*c3 ),
		vec4f( a.v[1]*c0, a.v[1]*c1, a.v[1]*c2, a.v[1]*c3 ),
		vec4f( a.v[2]*c0, a.v[2]*c1, a.v[2]*c2, a.v[2]*c3 ),
		vec4f( a.v[3]*c0, a.v[3]*c1, a.v[3]*c2, a.v[3]*c3 ) );
}

inline mat4f operator *( const mat4f& a, const double d )
{
	return mat4f( a.v[0]*d, a.v[1]*d, a.v[2]*d, a.v[3]*d );
}

inline mat4f operator *( const double d, const mat4f& a )
{
	return mat4f( d*a.v[0], d*a.v[1], d*a.v[2], d*a.v[3] );
}

inline mat4f operator /( const mat4f& a, const double d )
{
	return mat4f( a.v[0]/d, a.v[1]/d, a.v[2]/d, a.v[3]/d );
}

inline bool operator ==( const mat4f& a, const mat4f& b )
{
	return a.v[0]==b.v[0] && a.v[1]==b.v[1] && a.v[2]==b.v[2] && a.v[3]==b.v[3];
}

inline bool operator !=( const mat4f& a, const mat4f& b )
{
	return !( a == b );
}

inline ostream& operator <<( ostream& os, const mat4f& m )
{
	os << m.v[0] << " " << m.v[1] << " " << m.v[2] << " " << m.v[3];
}

inline istream& operator >>( istream& is, mat4f& m )
{
	is >> m.v[0] >> m.v[1] >> m.v[2] >> m.v[3];
}

inline void swap( mat4f& a, mat4f& b )
{
	swap( a.v[0], b.v[0] );
	swap( a.v[1], b.v[1] );
	swap( a.v[2], b.v[2] );
	swap( a.v[3], b.v[3] );
}

inline vec3f::vec3f( const vec4f& v ) 
{ 
	n[0] = v[0]; 
	n[1] = v[1]; 
	n[2] = v[2]; 
}
/*
inline vec3f clamp( const vec3f& other )
{
	return maximum( vec3f(), minimum( other, vec3f( 1.0, 1.0, 1.0 ) ) );
}
*/
#endif // __VECMATH_H__
