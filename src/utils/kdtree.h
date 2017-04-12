#pragma once
#ifndef _KDTREE_H_
#define _KDTREE_H_
#include "../vecmath/vecmath.h"
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;
enum level { X = 0, Y, Z };

struct photon {
	vec3f position;
	vec3f direction;
	vec3f energy;
};

struct range {
	double xRange[2];
	double yRange[2];
	double zRange[2];
};

class kdNode {

private:
	kdNode* left;
	kdNode* right;
	float	value;
	level	currentLevel;
	static kdNode* processList(vector<photon*>& list, int start, int begin, level nextLevel);
public:
	kdNode() {
		left = NULL;
		right = NULL;
	}
	kdNode(level level) {
		currentLevel = level;
		right = NULL;
		left = NULL;
	}
	
	static kdNode* processList(vector<photon*>& list);
	virtual void getPhoton(range* range,vector<photon*>& lsit, level currentLevel);
	~kdNode() {
		if (left)
			delete left;
		if (right)
			delete right;
	}
};

class kdLeaf :public kdNode {
private:
	photon* p;
public:
	virtual void getPhoton(range* range,vector<photon*>& list, level currentLevel);
	kdLeaf(photon* p):kdNode() {
		this->p = p;
	}
};
#endif