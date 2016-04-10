#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <string>


class ModelPLY
{

	float* faces;
	float* vertexBuffer;
	float* normals;

	int totalConnectedTriangles;
	int totalConnectedPoints;
	int totalFaces;

public:

	ModelPLY();
	~ModelPLY();

	int Load(char *filename);
	float* CalculateNormal(float *coord1, float *coord2, float *coord3);
};