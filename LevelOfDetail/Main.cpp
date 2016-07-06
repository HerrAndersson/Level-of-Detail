#include "stdafx.h"
#include "LevelOfDetail.h"
#include <exception>
#include <time.h>
#include "Console.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	RedirectIOToConsole();
#endif

	srand((unsigned int)time(0));

	try
	{
		LevelOfDetail sample(SCREEN_WIDTH, SCREEN_HEIGHT, L"D3D11 - LevelOfDetail");
		return Win32Application::Run(&sample, hInstance, nCmdShow);
	}
	catch (const std::exception& e)
	{
		string s = e.what();
		wstring ws = wstring(s.begin(), s.end());
		MessageBox(NULL, ws.c_str(), L"Error", MB_ICONERROR | MB_DEFAULT_DESKTOP_ONLY);
	}
	return 0;
}


