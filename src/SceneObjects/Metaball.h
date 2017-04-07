#pragma once
#include "../vecmath/vecmath.h"
#include "../utils/MarchingCube.h"
#include <vector>
using namespace std;

class MaterialSceneObject;
class Geometry;
class ball :public MaterialSceneObject{
public:
	vec3f position;
	float radius;
	ball(float x, float y, float z, float newRadius,Scene* scene,Material* m):MaterialSceneObject(scene,m) {
		position[0] = x; position[1] = y; position[2] = z;
		radius = newRadius;
	}
};

class Metaball :public MaterialSceneObject{
private:
	vector<ball*> balls;
	grid grid;
	bool isInitialized;
	double size;
public:
	Metaball(int gridSize,int size, Material* m, Scene*scene,TransformNode* t);
	void addBalls(ball* ball);
	void drawMetaball();
	double getSize() { return size; }
	void setBallPosition(int index, vec3f newPosition);
};
