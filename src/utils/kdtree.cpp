#include "kdtree.h"

bool compareX(photon* x, photon* y) {
	//cout << x->position[0] << "," << y->position[0] << endl;
	if (x->position[0] < y->position[0])
		return true;
	else
		return false;
}

bool compareY(photon* x, photon* y) {
	if (x->position[1] < y->position[1])
		return true;
	else
		return false;
}
bool compareZ(photon* x, photon* y) {
	if (x->position[2] < y->position[2])
		return true;
	else
		return false;
}

kdNode* kdNode::processList(vector<photon*>& list) {
	kdNode* k = new kdNode;
	//cout << "?";

	sort(list.begin(), list.end(),compareX);
	kdNode* current = k->processList(list, 0, list.size() - 1, X);
	delete k;
	return current;
}
kdNode* kdNode::processList(vector<photon*>& list, int start, int end, level type) {
	if (start == end) {
		//cout << list[start]->position << endl;
		return new kdLeaf(list[start]);
	}

	kdNode* node = new kdNode(X);
	int middle = (start + end) / 2;
	/*for (int i = start; i <= end; i++) {
		cout << list[i]->position << endl;
	}
	cout << endl;
	//cout << "middle:" << middle << endl;
	//cout << "start position:" << list[start]->position << endl;
	//cout << "middle position:" << list[middle]->position << endl;
	//cout << "end position:" << list[end]->position << endl;
	*/
	switch (type) {
	case X://current level is x, sort for the next level
		node->value = list[middle]->position[0];
		sort(list.begin() + start, list.begin() + middle + 1, compareY);
		sort(list.begin() + middle + 1, list.begin() + end + 1, compareY);
		node->left = processList(list, start, middle, Y);
		node->right = processList(list, middle + 1, end, Y);
		break;
	case Y:
		node->value = list[middle]->position[1];
		sort(list.begin() + start, list.begin() + middle + 1, compareZ);
		sort(list.begin() + middle + 1, list.begin() + end + 1, compareZ);
		node->left = processList(list, start, middle, Z);
		node->right = processList(list, middle + 1, end, Z);
		break;
	case Z:
		node->value = list[middle]->position[2];
		sort(list.begin() + start, list.begin() + middle + 1, compareX);
		sort(list.begin() + middle + 1, list.begin() + end + 1, compareX);
		node->left = processList(list, start, middle, X);
		node->right = processList(list, middle + 1, end, X);
		break;
	default:
		cerr << "the type is wrong!" << endl;
		break;
	}

	return node;
}

void kdNode::getPhoton(range* range,vector<photon*>& list, level currentType) {

	switch (currentType) {
	case X:
		if (value < range->xRange[0]) {
			right->getPhoton(range, list, Y);
		}
		else if (range->xRange[0] <= value && range->xRange[1] > value) {
			left->getPhoton(range, list, Y);
			right->getPhoton(range, list, Y);
		}
		else {
			left->getPhoton(range, list, Y);
		}
		break;
	case Y:
		if (value < range->yRange[0]) {
			right->getPhoton(range, list, Z);
		}
		else if (value >= range->yRange[0] && value < range->yRange[1]) {
			left->getPhoton(range, list, Z);
			right->getPhoton(range, list, Z);
		}
		else {
			left->getPhoton(range, list, Z);
		}
		break;
	case Z:
		if (value < range->zRange[0]) {
			right->getPhoton(range, list, X);
		}
		else if (value >= range->zRange[0] && value < range->zRange[1]) {
			left->getPhoton(range, list, X);
			right->getPhoton(range, list, X);
		}
		else {
			left->getPhoton(range, list, X);
		}
		break;
	default:
		cerr << "wrong input type!" << endl;
		break;
	}

}

void kdLeaf::getPhoton(range* range, vector<photon*>& list, level currentType) {
	bool ok = true;
	if (p->position[0] < range->xRange[0] || p->position[0] > range->xRange[1]) {
		ok = false;
	}
	if (p->position[2] < range->zRange[0] || p->position[2] > range->zRange[1]){
		ok = false;
	}

	if (p->position[1] < range->yRange[0] || p->position[1] > range->yRange[1]){
			ok = false;
	}

	if (ok)
		list.push_back(p);
}
