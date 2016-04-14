#include "MouseHandler.h"

MouseHandler::MouseHandler() : mousePos(0, 0), deltaPos(0, 0)
{
}

MouseHandler::~MouseHandler()
{
}

void MouseHandler::Update()
{
	HWND hwnd = Win32Application::GetHwnd();

	POINT point;
	GetCursorPos(&point);

	RECT rect;
	GetClientRect(hwnd, &rect);
	MapWindowPoints(hwnd, GetParent(hwnd), (LPPOINT)&rect, 2);
	
	float sx = float(rect.left + rect.right) / 2.0f;
	float sy = float(rect.top + rect.bottom) / 2.0f;
	SetCursorPos((int)sx, (int)sy);

	float dx = point.x - sx;
	float dy = point.y - sy;

	deltaPos = float2(dx, dy);

	if (mousePos.x < 0) { mousePos.x = 0; }
	if (mousePos.y < 0) { mousePos.y = 0; }
	if (mousePos.x  > SCREEN_WIDTH) { mousePos.x = (float)SCREEN_WIDTH; }
	if (mousePos.y  > SCREEN_HEIGHT) { mousePos.y = (float)SCREEN_HEIGHT; }
}

bool MouseHandler::MouseMoved(float2& difference)
{
	if (std::abs(deltaPos.x) < F_EPSILON && std::abs(deltaPos.y) < F_EPSILON)
	{
		difference.x = 0;
		difference.y = 0;

		return false;
	}
	else
	{
		difference = deltaPos;
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


