#pragma once
#ifndef _PARTICLE_SYSTEM_H_
#define _PARTICLE_SYSTEM_H_
#include "../scene/scene.h"
#include "../vecmath/vecmath.h"
#include <algorithm>
#include <random>
#include <stack>

#define EBSILON = 0.00001;
struct particle {
	vec3f currentColor;
	vec3f speed;
	vec3f position;
	vec3f deltaColor;
	int life;
};

class ParticleSystem :public MaterialSceneObject {
private:
	vec3f direction;
	vec3f startColorMin;
	vec3f startColorMax;
	vec3f endColorMin;
	vec3f endColorMax;
	vec3f force;
	int minLife;
	int maxLife;
	int numEmit;//number of particles emitted per frame;
	int maxNumParticles;
	int currentNumParticles;
	float maxSpeed;
	float minSpeed;
	vector<particle*> particles;//point to the first particle in particle list
	stack<particle*> recycleBin;//point to the first particle in recycle bin
	vec3f getDir(float theta,float phi);
	void randomize(particle* p);
	void addParticles();
	bool equal(vec3f& postionParticle, vec3f& positionRay, vec3f& direction)const {
		vec3f dir = (postionParticle - positionRay).normalize();
		if (abs(abs(dir * direction) - 1) < 0.00001)
			return true;
		return false;
	}
	void initParticleSystem();
	void updateParticles();
	void emit();

public:
	ParticleSystem(vec3f direction, vec3f startColorMin, vec3f startColorMax, vec3f endColorMin, vec3f endColorMax, float force, int minLife, int maxLife, int numEmit, int maxNumParticles, float maxSpeed, float minSpeed, Scene *scene, Material *mat) :MaterialSceneObject(scene, mat) {
		this->direction = direction;
		this->startColorMin = startColorMin;
		this->startColorMax = startColorMax;
		this->endColorMin = endColorMin;
		this->endColorMax = endColorMax;
		this->force = direction * force;
		this->minLife = minLife;
		this->maxLife = maxLife;
		this->numEmit = numEmit;
		this->maxSpeed = maxSpeed;
		this->minSpeed = minSpeed;
		this->maxNumParticles = maxNumParticles;
		initParticleSystem();
	}
	virtual bool intersectLocal(const ray& r, isect& i) const;
	virtual bool hasBoundingBoxCapability() const { return false; }
	virtual vec3f getTextureColor(vec3f& interxecPoint) { return vec3f(0, 0, 0); }
	~ParticleSystem() {
		while (!recycleBin.empty()) {
			particle* p = recycleBin.top();
			recycleBin.pop();
			delete p;
		}

		for (particle* p : particles)
			delete p;
	}
};

#endif