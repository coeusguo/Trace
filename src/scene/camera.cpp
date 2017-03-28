#include "camera.h"

#define PI 3.14159265359
#define SHOW(x) (cerr << #x << " = " << (x) << "\n")

Camera::Camera()
{
    aspectRatio = 1;
    normalizedHeight = 1;
    
    eye = vec3f(0,0,0);
    u = vec3f( 1,0,0 );
    v = vec3f( 0,1,0 );
    look = vec3f( 0,0,-1 );
}

void
Camera::rayThrough( double x, double y, ray &r )
// Ray through normalized window point x,y.  In normalized coordinates
// the camera's x and y vary both vary from 0 to 1.
{
    x -= 0.5;
    y -= 0.5;
    vec3f dir = look + x * u + y * v;
    r = ray( eye, dir.normalize() );
}

void
Camera::setEye( const vec3f &eye )
{
    this->eye = eye;
}

void
Camera::setLook( double r, double i, double j, double k )
// Set the direction for the camera to look using a quaternion.  The
// default camera looks down the neg z axis with the pos y axis as up.
// We derive the new look direction by rotating the camera by the
// quaternion rijk.
{
                                // set look matrix
    m[0][0] = 1.0 - 2.0 * (i * i + j * j);
    m[0][1] = 2.0 * (r * i - j * k);
    m[0][2] = 2.0 * (j * r + i * k);
    
    m[1][0] = 2.0 * (r * i + j * k);
    m[1][1]= 1.0 - 2.0 * (j * j + r * r);
    m[1][2] = 2.0 * (i * j - r * k);
    
    m[2][0] = 2.0 * (j * r - i * k);
    m[2][1] = 2.0 * (i * j + r * k);
    m[2][2] = 1.0 - 2.0 * (i * i + r * r);

    update();
}

void
Camera::setLook( const vec3f &viewDir, const vec3f &upDir )
{
    vec3f z = -viewDir;          // this is where the z axis should end up
    const vec3f &y = upDir;      // where the y axis should end up
    vec3f x = y.cross(z);               // lah,

    m = mat3f( x,y,z ).transpose();

    update();
}

void
Camera::setFOV( double fov )
// fov - field of view (height) in degrees    
{
    fov /= (180.0 / PI);      // convert to radians
    normalizedHeight = 2 * tan( fov / 2 );
    update();
}

void
Camera::setAspectRatio( double ar )
// ar - ratio of width to height
{
    aspectRatio = ar;
    update();
}

void
Camera::update()
{
    u = m * vec3f( 1,0,0 ) * normalizedHeight*aspectRatio;
    v = m * vec3f( 0,1,0 ) * normalizedHeight;
    look = m * vec3f( 0,0,-1 );
}




