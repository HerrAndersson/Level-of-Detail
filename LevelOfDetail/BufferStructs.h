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

struct CBPerPatchHS
{
	float distanceToCamera;
	float tessellationFactor;
	XMFLOAT2 pad;
};