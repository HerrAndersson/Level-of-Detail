#include "stdafx.h"
#include "LevelOfDetail.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	LevelOfDetail sample(SCREEN_WIDTH, SCREEN_HEIGHT, L"D3D11 - LevelOfDetail");
	return Win32Application::Run(&sample, hInstance, nCmdShow);
}
