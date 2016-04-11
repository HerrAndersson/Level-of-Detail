#pragma once
#include "stdafx.h"
using namespace Math;

struct CBPerFrame
{
	XMMATRIX view;
	XMMATRIX projection;
};

struct CBPerObject
{
	XMMATRIX world;
};