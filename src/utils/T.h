#pragma once

#ifndef _T_H_
#define _T_H_

#include <vector>
#include <algorithm>
const double EBSILON = 0.00001;
using namespace std;

class T {
private:
	vector<float> ts;
public:
	T() {}
	T(float min, float max) {
		ts.push_back(min);
		ts.push_back(max);
	}
	void Intersect(T& t);
	void Union(T& t);
	void Minus(T& t);
	void addTuple(T& t);
	float operator[](int i);
	T firstTuple();
	void eat();
	int size() { return ts.size(); }
	void clear() { ts.clear(); }
};

#endif // !_T_H_
