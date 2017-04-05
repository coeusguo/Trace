#pragma once

#ifndef _OCTREE_H_
#define _OCTREE_H_

#include <algorithm>
#include <list>
#include "../scene/ray.h"
#include "../scene/BoundingBox.h"
#include "../scene/scene.h"
#include <vector>

class Geometry;
class Scene;

//because of the stupid circular including problem
//I have to move the declaration of the class to scene.h

#endif