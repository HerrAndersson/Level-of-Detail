#pragma once
#include "stdafx.h"
using namespace Math;

struct CBPerFrame
{
	XMMATRIX view;
	XMMATRIX projection;
};

struct CBPerObjectVS
{
	XMMATRIX world;
};

struct CBPerObjectPS
{
	XMFLOAT3 color;
	float blendFactor;
	int hasTexture;
	XMFLOAT3 pad;
};

struct CBPerObjectHS
{
	float3 cameraPosition;
	float tessellationFactor;
	float3 viewVector;
	float minDistance;
	float range;
	XMFLOAT3 pad;
};