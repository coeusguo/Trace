#include "MarchingCube.h"
#include <math.h>
#include <iostream>
using namespace std;
grid::grid(int gridSize,int size,Material* m,Scene* scene, TransformNode* t) {
	numVertices = pow(gridSize + 1, 3);
	numCubes = pow(gridSize, 3);
	vertices = new vertex[numVertices];
	cubes = new cube[numCubes];
	//cout << "numVertices:" << numVertices << "," << "numCubes" << numCubes << endl;
	
	//all ther vertices share the same material
	sharedMaterial = m;
	

	mesh = new Trimesh(scene, m, t);
	this->scene = scene;

	//initialize the position of the vertices
	int currentVertex = 0;
	for (float Z = 0.0; Z < gridSize + 1; Z++) {
		for (float Y = 0.0; Y < gridSize + 1; Y++) {
			for (float X = 0.0; X < gridSize + 1; X++) {
				vertices[currentVertex].normal = vec3f(0.0, 0.0, 0.0);

				vertices[currentVertex].position[0] = (X / gridSize) * size * 2 - size;
				vertices[currentVertex].position[1] = (Y / gridSize) * size * 2 - size;
				vertices[currentVertex].position[2] = (Z / gridSize) * size * 2 - size;
				vertices[currentVertex].value = 0.0f;

				currentVertex++;
			}
		}
	}

	int currentCube = 0;
	int vertexSize = gridSize + 1;
	for (int Z = 1; Z <= gridSize; Z++) {
		for (int Y = 1; Y <= gridSize; Y++) {
			for (int X = 1; X <= gridSize; X++) {
				cubes[currentCube].vertices[0] = &vertices[(X - 1) + (Y - 1) * vertexSize + (Z - 1) * vertexSize * vertexSize];
				cubes[currentCube].vertices[1] = &vertices[X + (Y - 1) * vertexSize + (Z - 1) * vertexSize * vertexSize];
				cubes[currentCube].vertices[2] = &vertices[X + (Y - 1) * vertexSize + Z * vertexSize * vertexSize];
				cubes[currentCube].vertices[3] = &vertices[(X - 1) + (Y - 1) * vertexSize + Z * vertexSize * vertexSize];
				cubes[currentCube].vertices[4] = &vertices[(X - 1) + Y * vertexSize + (Z - 1) * vertexSize * vertexSize];
				cubes[currentCube].vertices[5] = &vertices[X + Y * vertexSize + (Z - 1) * vertexSize * vertexSize];
				cubes[currentCube].vertices[6] = &vertices[X + Y * vertexSize + Z * vertexSize * vertexSize];
				cubes[currentCube].vertices[7] = &vertices[(X - 1) + Y * vertexSize + Z * vertexSize * vertexSize];
				currentCube++;
			}
		}
	}
}

grid::~grid() {
	delete[]vertices;
	delete[]cubes;
	delete mesh;
}

void grid::drawSurface(float isolevel) {
	surfaceVertex onEdge[12];


	for (int i = 0; i < numCubes; i++) {

		int cubeindex = 0;
		if (cubes[i].vertices[0]->value < isolevel) cubeindex |= 1;
		if (cubes[i].vertices[1]->value < isolevel) cubeindex |= 2;
		if (cubes[i].vertices[2]->value < isolevel) cubeindex |= 4;
		if (cubes[i].vertices[3]->value < isolevel) cubeindex |= 8;
		if (cubes[i].vertices[4]->value < isolevel) cubeindex |= 16;
		if (cubes[i].vertices[5]->value < isolevel) cubeindex |= 32;
		if (cubes[i].vertices[6]->value < isolevel) cubeindex |= 64;
		if (cubes[i].vertices[7]->value < isolevel) cubeindex |= 128;

		if (cubeindex == 0 || cubeindex == 255) {
			continue;

		}

		int edges = edgeTable[cubeindex];
		int bit = 0;

		for (int n = 0; n < 12; n++) {
			bit = 1 << n;
			if (edges & bit) {
				vertex* p1 = cubes[i].vertices[endPoint[n * 2]];
				vertex* p2 = cubes[i].vertices[endPoint[n * 2 + 1]];

				float factor = (isolevel - p1->value) / (p2->value - p1->value);

				onEdge[n].position[0] = p1->position[0] + factor * (p2->position[0] - p1->position[0]);
				onEdge[n].position[1] = p1->position[1] + factor * (p2->position[1] - p1->position[1]);
				onEdge[n].position[2] = p1->position[2] + factor * (p2->position[2] - p1->position[2]);

				onEdge[n].normal[0] = p1->normal[0] + factor * (p2->normal[0] - p1->normal[0]);
				onEdge[n].normal[1] = p1->normal[1] + factor * (p2->normal[1] - p1->normal[1]);
				onEdge[n].normal[2] = p1->normal[2] + factor * (p2->normal[2] - p1->normal[2]);
			}
		}
		
		//cout << cubeindex << endl;
		int k = 0;
		for (int j = 0; triTable[cubeindex][j * 3] != -1; j++) {
			int n = mesh->getNumberOfVertices();
			//cout << n << endl;
			Material* m = new Material;
			m = sharedMaterial;

			vec3f p1 = onEdge[triTable[cubeindex][j * 3]].position;
			vec3f n1 = onEdge[triTable[cubeindex][j * 3]].normal;
			mesh->addVertex(p1);
			mesh->addMaterial(m);
			//mesh->addNormal(n1);
			//cout << onEdge[triTable[cubeindex][j * 3]].position << endl;
			m = new Material;
			m = sharedMaterial;
			vec3f p2 = onEdge[triTable[cubeindex][j * 3 + 1]].position;
			vec3f n2 = onEdge[triTable[cubeindex][j * 3 + 1]].normal;
			mesh->addVertex(p2);
			mesh->addMaterial(m);
			//mesh->addNormal(n2);

			//cout << onEdge[triTable[cubeindex][j * 3 + 1]].position << endl;
			m = new Material;
			m = sharedMaterial;
			vec3f p3 = onEdge[triTable[cubeindex][j * 3 + 2]].position;
			vec3f n3 = onEdge[triTable[cubeindex][j * 3 + 2]].normal;
			mesh->addVertex(p3);
			mesh->addMaterial(m);
			//mesh->addNormal(n3);
			//cout << onEdge[triTable[cubeindex][j * 3 + 2]].position << endl;
			//cout << "n:" << n << endl;
			//cout << endl;
			cout << k << endl;
			k++;
		}

	
		
		char* s =  mesh->doubleCheck();
		if (s)
			cout << s << "," << "!!!";
	}
	for (int i = 0; i < mesh->getNumberOfVertices(); i++) {
		mesh->addFace(i * 3, i * 3 + 1, i * 3 + 2);
	}
	//mesh->outputVertices();
}