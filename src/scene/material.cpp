#include "ray.h"
#include "material.h"
#include "light.h"

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.

	typedef list<Light*>::const_iterator 	cliter;
	cliter l;
	vec3f normal = i.N;
	vec3f point = r.at(i.t);
	vec3f color = ke + ka * scene->getAmbientLight();

	for (l = scene->beginLights(); l != scene->endLights(); ++l) {
		vec3f lightDirection = (*l)->getDirection(point);

		float fatt = (*l)->distanceAttenuation(point);//distance attenuation
		vec3f satt = (*l)->shadowAttenuation(point);
		vec3f i = (*l)->getColor(point);//the intensity of current light
		float diffuse = normal*lightDirection;//N * L
		if (diffuse < 0)
			diffuse = 0;

		vec3f ref = lightDirection - lightDirection * normal * 2 * normal;//the reflection of the light direction vector
		ref = ref.normalize();
		float spec = ref * r.getDirection();
		if (spec < 0)
			spec = 0;
		spec = pow(spec, shininess);

		vec3f result = (kd * diffuse + ks * spec) * fatt;
		for (int k = 0; k < 3; k++)
			result[k] = result[k] * i[k] * satt[k];

		color += result;
	}

	for (int i = 0; i < 3; i++)
		if (color[i] > 255)
			color[i] = 255;
	vec3f trans(1.0, 1.0, 1.0);
	trans -= i.getMaterial().kt;
	color = prod(color, trans);
	return color;
}
