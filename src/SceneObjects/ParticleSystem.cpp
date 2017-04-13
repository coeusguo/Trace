#include "ParticleSystem.h"

void ParticleSystem::initParticleSystem() {
	particle* b = new particle;
	recycleBin.push(b);
	for (int i = 1; i < maxNumParticles; i++) {
		b = new particle;
		recycleBin.push(b);
	}

	addParticles();
	emit();
	/*int i = 0;
	particle* pree = particles;
	while (pree) {
		i++;
		pree = pree->next;
	}
	cout << i << endl;
	i = 0;
	pree = recycleBin;
	while (pree) {
		i++;
		pree = pree->next;
	}
	cout << i << endl;*/
}

vec3f ParticleSystem::getDir(float theta, float phi) {
	theta = theta * 2 * 3.1416;
	phi = phi * 2 * 3.1416;
	//cout << "theta" << "," << phi << endl;
	return vec3f(sinf(theta) * sinf(phi), cosf(theta), sinf(theta)* cosf(phi)).normalize();
}

void ParticleSystem::randomize(particle* p) {
	if (!p)
		return;
	uniform_real_distribution<double> rand(0, 1);
	static default_random_engine re;

	p->currentColor = rand(re) * (startColorMax - startColorMin) + startColorMin;
	//cout << "color" << p->currentColor << endl;
	p->life = rand(re) * (maxLife - minLife) + minLife;
	//cout << p->life << endl;
	// the color of the particle is assumed to fate out eventually
	p->deltaColor = (p->currentColor - (rand(re) * (endColorMax - endColorMin) + endColorMin)) / float(p->life);
	float theta = rand(re);
	float phi = rand(re);
	p->position = getDir(theta, phi) * 0.01;
	p->speed = p->position.normalize() * ((maxSpeed - minSpeed) * rand(re) + minSpeed);
	//cout << p->speed << endl;
}

void ParticleSystem::updateParticles() {
	vector<particle*> newList;
	particle* p;
	while (!particles.empty()) {
		p = particles.back();
		particles.pop_back();
		if (p->life != 0) {
			p->position = p->position + p->speed;
			p->speed = p->speed + force;
			p->currentColor = p->currentColor - p->deltaColor; // the color of the particle is assumed to fate out eventually
			p->life--;
			newList.push_back(p);
		}
		else {
			recycleBin.push(p);
		}

	}
	particles = newList;
	addParticles();
}

void ParticleSystem::addParticles() {
	particle* p;
	for (int i = 0; i < numEmit; i++) {
		if (recycleBin.empty())
			break;
		p = recycleBin.top();
		recycleBin.pop();
		randomize(p);
		particles.push_back(p);
	}
}

bool ParticleSystem::intersectLocal(const ray& r, isect& i)const {

	bool haveOne = false;
	vec3f position = r.getPosition();
	vec3f dir = r.getDirection();
	i.t = 0;
	particle* one = NULL;
	for(int k = 0;k < particles.size();k++) {
		particle* p = particles[k];
		if (equal(p->position, position, dir)) {
			float curT = (p->position - position).length();
			if (!haveOne || curT < i.t) {
				i.t = curT;
				haveOne = true;
				one = p;
			}
		}
	}

	Material* m = new Material;
	
	if (one) {
		i.N = one->speed.normalize();
		m->kd = one->currentColor;
		i.material = m;
	}
	return haveOne;
}

void ParticleSystem::emit() {
	for (int i = 0; i < 80; i++) 
		updateParticles();
}

