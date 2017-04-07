#include "Octree.h"

OctNode::OctNode(OctNode* parent, vec3f& min, vec3f& max, int depth) {
	this->parent = parent;
	this->min = min;
	this->max = max;
	empty = true;
	hasChildren = false;

	if(depth >= 1){
		depth--;
		hasChildren = true;
		initChildren(depth);
	}
}

void OctNode::initChildren(int depth) {
	float deltaX = (max[0] - min[0]) / 2.0f;
	float deltaY = (max[1] - min[1]) / 2.0f;
	float deltaZ = (max[2] - min[2]) / 2.0f;
	//cout << deltaX << "," << deltaY << "," << deltaZ << endl;
	vec3f newMin, newMax;
	//children 1
	newMin = min;
	newMax = newMin;
	newMax[0] += deltaX; newMax[1] += deltaY; newMax[2] += deltaZ;
	childrens[0] = new OctNode(this, newMin, newMax, depth);

	//children2
	newMin = min;
	newMin[0] += deltaX;
	newMax = newMin;
	newMax[0] += deltaX; newMax[1] += deltaY; newMax[2] += deltaZ;
	childrens[1] = new OctNode(this, newMin, newMax, depth);

	//children3
	newMin = min;
	newMin[2] += deltaZ;
	newMax = newMin;
	newMax[0] += deltaX; newMax[1] += deltaY; newMax[2] += deltaZ;
	childrens[2] = new OctNode(this, newMin, newMax, depth);

	//children4
	newMin = min;
	newMin[0] += deltaX; newMin[2] += deltaZ;
	newMax = newMin;
	newMax[0] += deltaX; newMax[1] += deltaY; newMax[2] += deltaZ;
	childrens[3] = new OctNode(this, newMin, newMax, depth);

	//children5
	newMin = min;
	newMin[1] += deltaY;
	newMax = newMin;
	newMax[0] += deltaX; newMax[1] += deltaY; newMax[2] += deltaZ;
	childrens[4] = new OctNode(this, newMin, newMax, depth);

	//children6
	newMin = min;
	newMin[0] += deltaX; newMin[1] += deltaY;
	newMax = newMin;
	newMax[0] += deltaX; newMax[1] += deltaY; newMax[2] += deltaZ;
	childrens[5] = new OctNode(this, newMin, newMax, depth);

	//children7
	newMin = min;
	newMin[1] += deltaY; newMin[2] += deltaZ;
	newMax = newMin;
	newMax[0] += deltaX; newMax[1] += deltaY; newMax[2] += deltaZ;
	childrens[6] = new OctNode(this, newMin, newMax, depth);

	//children8
	newMin = min;
	newMin[0] += deltaX; newMin[1] += deltaY; newMin[2] += deltaZ;
	newMax = max;
	childrens[7] = new OctNode(this, newMin, newMax, depth);
}

void OctNode::processObject(Geometry* object) {
	//cout << "ab" << endl;
	object->ComputeBoundingBox();
	BoundingBox obb = object->getBoundingBox();

	int numIntersection = 0;//number of nodes that current objects intersects with
	int index[] = { 8,8,8,8,8,8,8,8 };
	int ini = 0;
	
	if (!hasChildren) {//base case
		if ((intersects(obb.min) && intersects(obb.max)) || obb.intersects(*this)) {
			objects.push_back(object);
			//cout << "??" << endl;
			empty = false;
			return;
		}
		else 
			return;
	}

	for (int i = 0; i < 8; i++) {
		bool state1 = childrens[i]->intersects(obb.min);
		bool state2 = childrens[i]->intersects(obb.max);
		bool state3 = childrens[i]->intersects(obb);
		bool state4 = (state1 && !state2) || (!state1 && state2) || (!state1 && !state2);
		if (state1 && state2) {//current box contains the object
			//cout << "hello" << "," << i << endl;
			empty = false;
			childrens[i]->processObject(object);
		}
		else if (state3 && state4){//the box intersects with current children
			index[ini] = i;
			ini++;
			numIntersection++;
			//cout << numIntersection << "," << i << endl;
			if (numIntersection > 3) {
				objects.push_back(object);
				empty = false;
				//cout << "?" << endl;
				return;
			}

		}else
			continue;
	}
	//cout << "ha" << endl;
	if (numIntersection != 0) {
		for (int i = 0; i < ini; i++)
			childrens[index[i]]->processObject(object);
		empty = false;
	}
	
}

bool OctNode::intersectNode(const ray& r, isect& i) {
	bool have_one = false;
	if(!hasChildren){//base case
		if (objects.empty())
			return false;
		else {
			for (cgiter it = objects.begin(); it != objects.end(); it++) {
				isect cur;
				if ((*it)->intersect(r, cur)) {
					if (!have_one ||cur.t < i.t) {
						i = cur;
						have_one = true;
						//cout << "yeah!!" << endl;
					}
				}
			}
			return have_one;
		}
	}
	
	//current level objects
	bool intersectLocalObject = false;
	i.t = 10000000.0;
	if (!objects.empty()) {//the object list of current tree level(the objects that intersect with more than 4 childrens)
		for (cgiter it = objects.begin(); it != objects.end(); it++) {
			isect cur;
			if ((*it)->intersect(r, cur)) {
				if (!have_one||cur.t < i.t) {
					i = cur;
					if (!intersectLocalObject) {
						intersectLocalObject = true;
						have_one = true;
					}
				}
			}
		}
	}

	vector<OctNode*> minNodes;
	for (int k = 0; k < 8; k++) {
		double tmin, tmax;//tmax is useless
		if (childrens[k]->intersect(r, tmin, tmax)) {
			//cout << "?";
			//if (tmin < RAY_EPSILON) {//the ray position is inside current children
				//tmin = tmax;
				/*if (!childrens[k]->isEmpty()) {
					isect ti;
					if (childrens[k]->intersectNode(r, ti)) {
						if (ti.t < i.t) {
							i = ti;
							return true;
						}
					}
					//cout << "yeah!" << endl;	
				}
			}*/
			//cout << tmin << ",";
			if ((tmin < i.t ||!have_one) && !childrens[k]->isEmpty()) {
				//cout << "?" << endl;
				minNodes.push_back(childrens[k]);
				have_one = true;
			}
		}
	}

	if (!have_one) {
		//cout << "wtf?!false?" << endl;
		return false;
	}
	//cout << "nani?" << endl;
	isect cur;
	bool ok = false;
	for (OctNode* o : minNodes) {
		if (o->intersectNode(r, cur)) {
			if (intersectLocalObject && cur.t < i.t) {
				i = cur;
				intersectLocalObject = false;//the object in current level is no longer the nearest one
				ok = true;
				continue;
			}
			if (!intersectLocalObject && cur.t < i.t) {
				i = cur;
				ok = true;
			}
		}
	}
	if (!intersectLocalObject && !ok) {
		//cout << "hello" << endl;
		return false;
	}

	//cout << "wtf";
	return true;
}

void Octree::processObjects(list<Geometry*> objects) {
	for (cgiter it = objects.begin(); it != objects.end(); it++) {
		if((*it)->hasBoundingBoxCapability())
			processObject(*it);
	}
}

bool Octree::intersectThis(const ray& r, isect& i) {
	return intersectNode( r, i);
}

void Octree::processOneObject(Geometry* object) {
	processObject(object);
}

OctNode::~OctNode() {
	if(hasChildren)
		for (int i = 0; i < 8; i++)
			delete childrens[i];
}

Octree::~Octree() {
}