#pragma once

#include "DXSample.h"
#include "SimpleCamera.h"
#include "StepTimer.h"
#include "Math.h"
#include "DirectXHandler.h"
#include "ShaderHandler.h"
#include "AssetManager.h"
#include "BufferStructs.h"

using namespace DirectX;
using namespace Math;
using namespace Renderer;

using Microsoft::WRL::ComPtr;

class LevelOfDetail : public DXSample
{

private:

	SimpleCamera camera;
	StepTimer timer;
	DirectXHandler* dx;
	AssetManager am;

	matrix projectionMatrix;

	ID3D11Device* deviceRef;
	ID3D11DeviceContext* deviceContextRef;

	VertexShaderData* defaultVS;
	ID3D11PixelShader* defaultPS;
	ID3D11SamplerState* samplerWrap;

	ID3D11Buffer* cbPerObject;
	ID3D11Buffer* cbPerFrame;

	void LoadAssets();
	void LoadPipelineObjects();
	float RandomPercent();

	void SetCBPerObject(matrix world, float3 color, float blendFactor);
	void SetCBPerFrame(matrix view, matrix projection);

public:

	LevelOfDetail(UINT width, UINT height, std::wstring name);

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();
	virtual void OnKeyDown(UINT8 key);
	virtual void OnKeyUp(UINT8 key);

};
