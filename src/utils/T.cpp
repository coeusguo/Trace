#include "T.h"
#include <iostream>
using namespace std;
void T::Intersect(T& t) {
	if (ts.size() == 0 || t.ts.size() == 0) {
		ts.clear();
		return;
	}
	vector<float> newT;

	int sizeA = ts.size();
	int currentA = 0;

	int sizeB = t.ts.size();
	int currentB = 0;

	int size = (sizeA + sizeB) / 2;

	for (int i = 0; i < size; i++) {
		if (currentA >= sizeA || currentB >= sizeB)
			break;
		if (ts[currentA + 1] + EBSILON < t.ts[currentB]) {
			currentA += 2;
		}
		else if (t.ts[currentB + 1] + EBSILON < ts[currentA]) {
			currentB += 2;
		}
		else if (ts[currentA] + EBSILON < t.ts[currentB]) {
			newT.push_back(t.ts[currentB]);
			newT.push_back(min(ts[currentA + 1], t.ts[currentB + 1]));
			if (ts[currentA + 1] + EBSILON < t.ts[currentB + 1]) {
				t.ts[currentB] = ts[currentA + 1];
				currentA += 2;
			}
			else if(abs(ts[currentA + 1] - t.ts[currentB + 1]) < EBSILON){
				currentA += 2;
				currentB += 2;
				i++;
			}
			else {
				ts[currentA] = t.ts[currentB + 1];
				currentB += 2;
			}
		}
		else {
			newT.push_back(ts[currentA]);
			newT.push_back(min(ts[currentA + 1], t.ts[currentB + 1]));
			if (t.ts[currentB + 1] + EBSILON < ts[currentA + 1]) {
				ts[currentA] = t.ts[currentB + 1];
				currentB += 2;
			}
			else if (abs(ts[currentA + 1] - t.ts[currentB + 1]) < EBSILON) {
				currentA += 2;
				currentB += 2;
				i++;
			}
			else {
				t.ts[currentB] = ts[currentA + 1];
				currentA += 2;
			}
		}
	}
	ts = newT;
}

//A union B
void T::Union(T& t) {
	if (ts.size() == 0) {//A is empty
		ts = t.ts;
		return;
	}

	if (t.ts.size() == 0) {//B is empty
		return;
	}
//	cout << "sphere1:" << ts[0] << "," << "sphere" << t.ts[0] << ",";
	vector<float> newT;
	int sizeA = ts.size();
	int currentA = 0;

	int sizeB = t.ts.size();
	int currentB = 0;

	int size = (sizeA + sizeB) / 2;
	//cout << "size:" << size << endl;

	int realNum = -1;//the number of element in newT
	for (int i = 0; i < size; i++) {
		//cout << i << endl;
		if (currentA >= sizeA) {//A is empty now
			//cout << "A is empty" << endl;	
			//cout << "currentB:" << currentB << endl;
			if (t.ts[currentB] < newT[realNum] + EBSILON) {
				//cout << "A begin" << endl;
				newT[realNum] = t.ts[currentB + 1];
				//cout << "A end" << endl;
			}
			else {
				newT.push_back(t.ts[currentB]);
				newT.push_back(t.ts[currentB + 1]);
				realNum += 2;
			}
			currentB += 2;
		}
		else if (currentB >= sizeB) {//B is empty now
			//cout << "B is empty" << endl;
			if (ts[currentA] < newT[realNum] + EBSILON) {
				newT[realNum] = ts[currentA + 1];
			}
			else {
				newT.push_back(ts[currentA]);
				newT.push_back(ts[currentA + 1]);
				realNum += 2;
			}
			currentA += 2;
			
		}
		else if (ts[currentA] <= t.ts[currentB]) {//A::tmin <= B::tmin
			if (ts[currentA + 1] + EBSILON < t.ts[currentB]) {// A is seprate with B
				//cout << "ok?" << endl;
				if (newT.empty()) {
					newT.push_back(ts[currentA]);
					newT.push_back(ts[currentA + 1]);
					realNum += 2;
				}
				else if(ts[currentA] > newT[realNum] + EBSILON) {
					newT.push_back(ts[currentA]);
					newT.push_back(ts[currentA + 1]);
					realNum += 2;
				}
				else {
					newT[realNum] = ts[currentA + 1];
				}
				currentA += 2;
			}
			else {//A and B overlap
				if (newT.empty()) {
					//cout << "ok here" << endl;
					newT.push_back(ts[currentA]);
					newT.push_back(max(ts[currentA + 1], t.ts[currentB + 1]));
					realNum += 2;
				}
				else if (ts[currentA] > newT[realNum] + EBSILON) {
					newT.push_back(ts[currentA]);
					newT.push_back(max(ts[currentA + 1], t.ts[currentB + 1]));
					realNum += 2;
				}
				else{//A and previous one overlap
					newT[realNum] = max(ts[currentA + 1], t.ts[currentB + 1]);
				}
				currentA += 2;
				currentB += 2;
				i++;
			}
		}
		else {//B::tmin < A::tmin
			//cout << "ok2" << endl;
			if (t.ts[currentB + 1] + EBSILON < ts[currentA]) {//B is seprate with A
				if (newT.empty()) {
					newT.push_back(t.ts[currentB]);
					newT.push_back(t.ts[currentB + 1]);
					realNum += 2;
				}
				else if(t.ts[currentB] > newT[realNum] + EBSILON) {//B is also seprate with previous one or newT is empty
					newT.push_back(t.ts[currentB]);
					newT.push_back(t.ts[currentB + 1]);
					realNum += 2;
				}
				else{//B is overlap with previous one
					newT[realNum] = t.ts[currentB + 1];
				}
				currentB += 2;
			}
			else {//B and A overlap
				if (newT.empty()) {
					newT.push_back(t.ts[currentB]);
					newT.push_back(max(ts[currentA + 1], t.ts[currentB + 1]));
					realNum += 2;
				}
				else if(t.ts[currentB] > newT[realNum] + EBSILON) {//seprate
					newT.push_back(t.ts[currentB]);
					newT.push_back(max(ts[currentA + 1], t.ts[currentB + 1]));
					realNum += 2;
				}
				else{//B and previous one overlap
					newT[realNum] = max(t.ts[currentB + 1], ts[currentA + 1]);
				}
				currentA += 2;
				currentB += 2;
				i++;
			}
		}
	}

	ts = newT;
	//cout << "final t:" << ts[0] << endl;
}

void T::Minus(T& t) {
	if (ts.empty() || t.ts.empty())
		return;

	vector<float> newT;
	int sizeA = ts.size();
	int currentA = 0;

	int sizeB = t.ts.size();
	int currentB = 0;

	int size = (sizeA + sizeB) / 2;//number of T pairs in total

	for (int i = 0; i < size; i++) {
		if (currentA >= sizeA) //A is empty
			break;
		if (currentB >= sizeB) {//B is empty
			newT.push_back(ts[currentA]);
			newT.push_back(ts[currentA + 1]);
			currentA += 2;
			continue;
		}
		if (ts[currentA + 1] < t.ts[currentB]) {//A is seprate with B ,A::tmax < B::tmin
			newT.push_back(ts[currentA]);
			newT.push_back(ts[currentA + 1]);
			currentA += 2;
		}
		else if (t.ts[currentB + 1] < ts[currentA]) {//B is seprate with A , B::tmax < A::tmin
			currentB += 2;
		}
		else if (ts[currentA] + EBSILON < t.ts[currentB]) {
			newT.push_back(ts[currentA]);
			newT.push_back(t.ts[currentB]);
			if (ts[currentA + 1] + EBSILON > t.ts[currentB + 1]) {//A is overlap with B
				ts[currentA] = t.ts[currentB + 1];
				currentB += 2;
			}
			else {
				currentA += 2;
			}
		}
		else {
			if (ts[currentA + 1] + EBSILON < t.ts[currentB + 1]) {
				currentA += 2;
			}
			else {
				ts[currentA] = t.ts[currentB + 1];
				currentB += 2;
			}
		}
	
	}

	ts = newT;

}

float& T::operator[](int i) {
	return ts[i];
}

T T::firstTuple() {
	return T(ts[0], ts[1]);
}

void T::addTuple(T& tuple) {
	for (int i = 0; i < tuple.ts.size(); i++)
		ts.push_back(tuple.ts[i]);
}

void T::eat() {
	while (ts[0] == 0 && ts[1] == 0) {
		
	}
}