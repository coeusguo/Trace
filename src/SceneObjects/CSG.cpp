#include "CSG.h"

T Node::PrimitiveInterSections = T();
T Operator::getInterSectT(const ray& r, T& primitiveInterSections) {
	T leftT = left->getInterSectT(r, primitiveInterSections);
	T rightT = right->getInterSectT(r, primitiveInterSections);
	
	switch (myType) {
	case UNION:
		leftT.Union(rightT);
		//cout << "hello?" << endl;
		break;
	case INTERSECT:
		leftT.Intersect(rightT);
		break;
	case MINUS:
		leftT.Minus(rightT);
		break;
	default:
		break;
	}

	return leftT;
}

T LeafNode::getInterSectT(const ray& r, T& primitiveInterSections) {
	T t = primitive->getPrimitiveT(r);
	if (t.size() != 0)
		primitiveInterSections.addTuple(t);
	else
		primitiveInterSections.addTuple(T(-1, -1));
	return t;
}

bool CSG::intersectLocal(const ray& r, isect& i) const {
	//cout << "hello" << endl;
	T primitiveInterSections;
	T t = csgTree->getInterSectT(r, primitiveInterSections);
	if (t.size() == 0) {
		return false;
	}
	//cout << "in CSG:"<<t[0]<<endl;
	//cout << csgTree->PrimitiveInterSections.size() << endl;
	int k;
	for (k = 0; k < primitiveInterSections.size(); k = k + 2) {
		//cout<< "("<<csgTree->PrimitiveInterSections[k]<<","<<t[0]<<","<< csgTree->PrimitiveInterSections[k]<<")"<<endl;
		if (primitiveInterSections[k] < t[0] + EBSILON&& primitiveInterSections[k + 1] > t[0] - EBSILON) {
			//cout << "?" << endl;
			break;
		}
	}
	//cout << "here2" << endl;
	i.obj = primitives[k / 2];
	//cout << t[0] << endl;
	i.t = t[0];
	//cout << i.obj->getMaterial().kd << endl;
	vec3f dir = r.getDirection();
	vec3f pos = r.at(t[0]) - prod(dir, vec3f(0.001, 0.001, 0.001));
	
	ray newRay(pos, dir);
	isect newI;
	//cout<<"?"<<endl;
	for (k = 0; k < primitiveInterSections.size(); k = k + 2) {
		if (abs(primitiveInterSections[k] - t[0]) < EBSILON || abs(primitiveInterSections[k + 1] - t[0]) < EBSILON)
			break;
	}
	primitives[k / 2]->intersect(newRay, newI);
	
	
	i.N = newI.N;
	if (i.N * r.getDirection() > 0)
		i.N = -i.N;
	//cout << i.N << endl;
	return true;
}

BoundingBox CSG::ComputeLocalBoundingBox() {
	primitives[0]->ComputeBoundingBox();
	BoundingBox bb = primitives[0]->getBoundingBox();
	for (int i = 1; i < primitives.size(); i++) {
		primitives[i]->ComputeBoundingBox();
		bb.min = minimum(bb.min, primitives[i]->getBoundingBox().min);
		bb.max = maximum(bb.max, primitives[i]->getBoundingBox().max);
	}
	return bb;
}