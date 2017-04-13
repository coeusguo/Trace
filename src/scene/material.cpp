#include "ray.h"
#include "material.h"
#include "light.h"

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
int Material::numMaterials = 0;

vec3f Material::shade( Scene *scene, const ray& r, const isect& i ,bool enableSoftShadow) const
{

	typedef list<Light*>::const_iterator 	cliter;
	cliter l;
	vec3f normal = i.N;
	vec3f point = r.at(i.t);
	vec3f trans(1.0, 1.0, 1.0);
	trans -= kt;
	vec3f color = ke + prod(prod(ka, scene->getAmbientLight() * scene->getAmbientLightFactor()), trans);

	for (l = scene->beginLights(); l != scene->endLights(); ++l) {
		vec3f lightDirection = (*l)->getDirection(point);

		float fatt = (*l)->distanceAttenuation(point);//distance attenuation
		vec3f satt = (*l)->shadowAttenuation(point,enableSoftShadow);
		vec3f ii = (*l)->getColor(point);//the intensity of current light

		vec3f diffuseColor = kd;
		
		vec3f temp = ((MaterialSceneObject*)(i.obj))->getTextureColor(r.at(i.t));
		if (scene->getUsingTexture()) {
			diffuseColor = temp;
		}

		if (scene->getUsingBump())
			normal = i.obj->getBumpNormal();

		float diffuse = normal*lightDirection;//N * L
		if (diffuse < 0)
			diffuse = 0;

		vec3f ref = lightDirection - (lightDirection * normal * 2) * normal;//the reflection of the light direction vector
		ref = ref.normalize();

		float spec = ref * r.getDirection();
		if (spec < 0)
			spec = 0;
		spec = pow(spec, shininess * 128);

		vec3f result = (prod(diffuseColor * diffuse, trans) + ks * spec) * fatt;
		for (int k = 0; k < 3; k++)
			result[k] = result[k] * ii[k] * satt[k];

		color += result;
	}

	


	vec3f caustic(0.0, 0.0, 0.0);
	if (kt.length() == 0 && kr.length() == 0 && scene->getEnableCaustic() && scene->isPhotonMapLoaded()) {
		caustic = computeCaustic(scene, r.at(i.t), normal,r.getDirection());
	}
	color += caustic;

	for (int i = 0; i < 3; i++)
		if (color[i] > 1.0)
			color[i] = 1.0;

	return color;
}

vec3f Material::computeCaustic(Scene* scene, vec3f& pos, vec3f& normal,vec3f& viewDir)const {
	range* r = new range;
	r->xRange[0] = pos[0] - 0.05;
	r->xRange[1] = pos[0] + 0.05;
	r->yRange[0] = pos[1] - 0.05;
	r->yRange[1] = pos[1] + 0.05;
	r->zRange[0] = pos[2] - 0.05;
	r->zRange[1] = pos[2] + 0.05;

	vector<photon*> plist;
	scene->getPhotons(plist, r);
	//cout << result.size() << endl;

	vec3f result(0.0, 0.0, 0.0);
	for (photon* p : plist) {
		float nl = (-p->direction) * normal;
		if (nl < 0)
			nl = 0;

		vec3f reflec = -((-p->direction) * normal * 2 * normal + p->direction);
		float nh = reflec * viewDir;
		if (nh < 0)
			nh = 0;

		result += prod(nl * kd + pow(nh, shininess) * ks, p->energy);
	}

	result /= (200);
	return result;
}


