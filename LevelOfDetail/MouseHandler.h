#pragma once
#include "stdafx.h"
#include "Win32Application.h"
using namespace Math;

class MouseHandler
{
private:

	float2 mousePos;
	float2 deltaPos;

public:

	MouseHandler();
	virtual ~MouseHandler();

	void Update();
	bool MouseMoved(float2& deltaPos);

	float2 GetMousePos();
	float2 GetMousePosScreenSpace();

};

