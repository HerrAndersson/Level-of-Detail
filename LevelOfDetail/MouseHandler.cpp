#include "MouseHandler.h"

MouseHandler::MouseHandler() : mousePos(0,0), lastMousePos(0,0)
{
}

MouseHandler::~MouseHandler()
{
}

void MouseHandler::Update()
{
	lastMousePos = mousePos;

	POINT point;
	GetCursorPos(&point);

	ScreenToClient(Win32Application::GetHwnd(), &point);

	mousePos.x = (float)point.x;
	mousePos.y = (float)point.y;

	if (mousePos.x < 0) { mousePos.x = 0; }
	if (mousePos.y < 0) { mousePos.y = 0; }
	if (mousePos.x  > SCREEN_WIDTH) { mousePos.x = (float)SCREEN_WIDTH; }
	if (mousePos.y  > SCREEN_HEIGHT) { mousePos.y = (float)SCREEN_HEIGHT; }
}

bool MouseHandler::MouseMoved(float2& difference)
{
	if (std::abs(lastMousePos.x - mousePos.x) < F_EPSILON && std::abs(lastMousePos.y - mousePos.y) < F_EPSILON)
	{
		difference.x = 0;
		difference.y = 0;

		return false;
	}
	else
	{
		difference.x = mousePos.x - lastMousePos.x;
		difference.y = mousePos.y - lastMousePos.y;

		return true;
	}
}

float2 MouseHandler::GetMousePos()
{
	return mousePos;
}

float2 MouseHandler::GetMousePosScreenSpace()
{
	//Converting mouse position to screen space range [-1, 1]
	float x = 2 * (mousePos.x / (float)SCREEN_WIDTH) - 1;
	float y = -(2 * (mousePos.y / (float)SCREEN_HEIGHT) - 1);

	return float2(x, y);
}


