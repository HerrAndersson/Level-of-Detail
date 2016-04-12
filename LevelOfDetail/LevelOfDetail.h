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

	matrix4x4 projectionMatrix;

	ComPtr<ID3D11Device> deviceRef;
	ComPtr<ID3D11DeviceContext> deviceContextRef;

	VertexShaderData* defaultVS;
	ComPtr<ID3D11PixelShader> defaultPS;
	ComPtr<ID3D11SamplerState> samplerWrap;

	ID3D11Buffer* cbPerObject;
	ID3D11Buffer* cbPerFrame;


	//TEmp
	ID3D11Buffer* vertexBuffer;

	void LoadAssets();
	void LoadPipelineObjects();
	float RandomPercent();

	void SetCBPerObject(matrix4x4 world);
	void SetCBPerFrame(matrix4x4 view, matrix4x4 projection);

public:

	LevelOfDetail(UINT width, UINT height, std::wstring name);

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();
	virtual void OnKeyDown(UINT8 key);
	virtual void OnKeyUp(UINT8 key);

};
