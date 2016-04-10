#pragma once

#include "DXSample.h"
#include "SimpleCamera.h"
#include "StepTimer.h"
#include "Math.h"
#include "DirectXHandler.h"
#include "ShaderHandler.h"
#include "AssetManager.h"

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
	AssetManager am;

	matrix4x4 projectionMatrix;

	VertexShaderData* defaultVS;
	ComPtr<ID3D11PixelShader> defaultPS;
	ComPtr<ID3D11SamplerState> samplerWrap;

	void LoadAssets();
	void LoadShaders();
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
