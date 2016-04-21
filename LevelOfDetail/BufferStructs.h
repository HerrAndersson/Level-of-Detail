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
};

struct CBPerObjectHS
{
	float3 cameraPosition;
	float tessellationFactor;
	float minDistance;
	float range;
	XMFLOAT2 pad;
};