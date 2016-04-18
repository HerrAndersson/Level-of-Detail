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
	bool wireframeModeActive;
	float3 rotation;

	matrix projectionMatrix;
	matrix viewMatrix;
	matrix worldMatrix;

	Camera camera;
	Camera freelookCamera;
	StepTimer timer;
	DirectXHandler* dx;
	MouseHandler mouse;

	vector<LoDObject*> lodObjects;

	ID3D11Device* deviceRef;
	ID3D11DeviceContext* deviceContextRef;

	//Vertex shaders
	VertexShaderData* defaultVS;
	VertexShaderData* cpntVS;
	//Hull shaders
	ID3D11HullShader* cpntHS;
	ID3D11HullShader* phongHS;
	//Domain shaders
	ID3D11DomainShader* cpntDS;
	ID3D11DomainShader* phongDS;
	//Pixel shaders
	ID3D11PixelShader* defaultPS;
	ID3D11PixelShader* cpntPS;

	ID3D11SamplerState* samplerWrap;

	ID3D11Buffer* cbPerObjectVS;
	ID3D11Buffer* cbPerFrameVS;
	ID3D11Buffer* cbPerObjectPS;
	ID3D11Buffer* cbPerFrameDS;

	void LoadAssets();
	void LoadPipelineObjects();
	float RandomPercent();

	void SetCBPerObject(matrix world, float3 color, float blendFactor);
	void SetCBPerFrame(matrix view, matrix projection);

	void SetCPNTDataPerFrame(matrix view, matrix projection);
	void SetCPNTDataPerObject(matrix world, float3 color, float tessellationFactor);
	
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
