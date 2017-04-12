#pragma once
#ifndef  _CSG_H_
#define  _CSG_H_
#include "../scene/scene.h"
#include "../utils/T.h"

enum type { UNION = 0, INTERSECT, MINUS };

class Node {
public:
	static T PrimitiveInterSections;
protected:
	Node* left;
	Node* right;
	Node* parent;
	Node() {
		left = NULL;
		right = NULL;
		parent = NULL;
	}
	Node(Node* parent) {
		this->parent = parent;
	}
public:
	void setLeft(Node* node) {
		left = node;
	};
	void setRight(Node* node) {
		right = node;
	};
	virtual T getInterSectT(const ray& r,T& primitiveInterSections) = 0;
};

class Operator :public Node {
private:
	type myType;
public:
	Operator(type i) :Node() {
		myType = i;
	}
	virtual T getInterSectT(const ray& r, T& primitiveInterSections);
};

class LeafNode :public Node {
private:
	Geometry* primitive;
public:
	LeafNode(Node* parent, Geometry* pri) :Node(parent) {
		primitive = pri;
	}
	virtual T getInterSectT(const ray& r, T& primitiveInterSections);
};

class CSG :public MaterialSceneObject {
private:
	Node* csgTree;
	vector<MaterialSceneObject*> primitives;
public:
	CSG(Scene* scene, TransformNode* node, Material* mat) :MaterialSceneObject(scene, mat) {
		csgTree = NULL;
		setTransform(node);
	}
	void setTree(Node* tree) { csgTree = tree; }
	void setPrimitives(vector<MaterialSceneObject*> pris) {
		primitives = pris;
	}
	virtual bool intersectLocal(const ray& r, isect& i) const;
	virtual bool hasBoundingBoxCapability() const { return true; }
	virtual BoundingBox ComputeLocalBoundingBox();
};
#endif // ! _CSG_H_

