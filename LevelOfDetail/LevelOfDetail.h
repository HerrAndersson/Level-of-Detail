#pragma once

#include "DXSample.h"
#include "Camera.h"
#include "StepTimer.h"
#include "Math.h"
#include "DirectXHandler.h"
#include "ShaderHandler.h"
#include "AssetManager.h"
#include "BufferStructs.h"
#include "MouseHandler.h"

using namespace DirectX;
using namespace Math;
using namespace Renderer;

using Microsoft::WRL::ComPtr;

class LevelOfDetail : public DXSample
{

private:
	enum LoDTechnique { NO_LOD, STATIC, UNPOPPING, CPNT, PHONG };

	float3 colors[100];
	LoDTechnique activeTechnique;
	bool freelookCameraActive;
	float3 rotation;

	Camera camera;
	Camera freelookCamera;
	StepTimer timer;
	DirectXHandler* dx;
	MouseHandler mouse;

	matrix projectionMatrix;
	matrix viewMatrix;
	matrix worldMatrix;

	ID3D11Device* deviceRef;
	ID3D11DeviceContext* deviceContextRef;

	VertexShaderData* defaultVS;
	ID3D11PixelShader* defaultPS;
	ID3D11SamplerState* samplerWrap;

	ID3D11Buffer* cbPerObject;
	ID3D11Buffer* cbPerFrame;

	vector<LoDObject*> lodObjects;

	void LoadAssets();
	void LoadPipelineObjects();
	float RandomPercent();

	void SetCBPerObject(matrix world, float3 color, float blendFactor);
	void SetCBPerFrame(matrix view, matrix projection);
	
	void RenderNoLOD();
	void RenderStaticLOD();
	void RenderUnpoppingLOD();
	void RenderCPNTLOD();
	void RenderPhongLOD();

	void SetNoLOD();
	void SetStaticLOD();
	void SetUnpoppingLOD();
	void SetCPNTLOD();
	void SetPhongLOD();

public:

	LevelOfDetail(UINT width, UINT height, std::wstring name);

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();
	virtual void OnKeyDown(UINT8 key);
	virtual void OnKeyUp(UINT8 key);
};
