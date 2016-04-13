#pragma once
#include "stdafx.h"
#include "Win32Application.h"
using namespace Math;

class MouseHandler
{
private:

	float2 mousePos;
	float2 lastMousePos;

public:

	MouseHandler();
	virtual ~MouseHandler();

	void Update();
	bool MouseMoved(float2& difference);

	float2 GetMousePos();
	float2 GetMousePosScreenSpace();

};

