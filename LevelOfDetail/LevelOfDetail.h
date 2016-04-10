#pragma once

#include "DXSample.h"
#include "SimpleCamera.h"
#include "StepTimer.h"
#include "Math.h"
#include "DirectXHandler.h"
#include "ShaderHandler.h"

using namespace DirectX;
using namespace Math;
using namespace Renderer;

using Microsoft::WRL::ComPtr;

class LevelOfDetail : public DXSample
{

private:

	struct CBPerFrame
	{
		matrix4x4 view;
		matrix4x4 projection;
	};

	struct CBPerObject
	{
		matrix4x4 world;
	};

	SimpleCamera camera;
	StepTimer timer;
	DirectXHandler* dx;

	matrix4x4 projectionMatrix;

	void LoadPipeline();
	void LoadAssets();

	float RandomPercent();

public:

	LevelOfDetail(UINT width, UINT height, std::wstring name);

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();
	virtual void OnKeyDown(UINT8 key);
	virtual void OnKeyUp(UINT8 key);

};
