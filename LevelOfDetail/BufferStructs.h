#pragma once
#include "stdafx.h"
using namespace Math;

struct CBPerFrameVS
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