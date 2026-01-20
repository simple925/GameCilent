#pragma once
#include "AMesh.h"
class TempMesh
	: public Asset
{
private:
public:
	AMesh* CreateCircle();
	AMesh* CreateRectangle();
	AMesh* CreateTriangle();
public:
	TempMesh();
	~TempMesh();
};

