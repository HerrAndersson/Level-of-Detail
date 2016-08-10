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
#include "Profiling.h"

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
	int objectIndex;
	float tessellationMinDistance;
	float tessellationRange;
	float tessellationFactor;
	float range;
	float speed;
	float2 sphericalCoordDegrees;
	bool goForward;

	matrix projectionMatrix;
	matrix viewMatrix;
	matrix worldMatrix;

	Camera camera;
	Camera freelookCamera;
	StepTimer timer;
	DirectXHandler* dx;
	MouseHandler mouse;
	//Profiler profiler;

	vector<LoDObject*> lodObjects;

	ID3D11Device* deviceRef;
	ID3D11DeviceContext* deviceContextRef;

	//Vertex shaders
	VertexShaderData* defaultVS;
	VertexShaderData* tessellationVS;
	//Hull shaders
	ID3D11HullShader* cpntHS;
	ID3D11HullShader* phongHS;
	//Domain shaders
	ID3D11DomainShader* cpntDS;
	ID3D11DomainShader* phongDS;
	//Pixel shaders
	ID3D11PixelShader* defaultPS;

	//Sampler states
	ID3D11SamplerState* samplerWrap;

	//Buffers
	ID3D11Buffer* cbPerObjectVS;
	ID3D11Buffer* cbPerFrameVS;
	ID3D11Buffer* cbPerObjectPS;
	ID3D11Buffer* cbPerFrameDS;
	ID3D11Buffer* cbPerObjectHS;

	void LoadAssets();
	void LoadPipelineObjects();

	void SetCBPerObject(matrix world, float3 color, float blendFactor, int hasTexture);
	void SetCBPerFrame(matrix view, matrix projection);

	void SetTessellationCBPerFrame(matrix view, matrix projection);
	void SetTessellationCBPerObject(matrix world, float3 color, int hasTexture, int lodIndex);
	
	void RenderNoLOD(LoDObject* object);
	void RenderStaticLOD(LoDObject* object);
	void RenderUnpoppingLOD(LoDObject* object);
	void RenderCPNTLOD(LoDObject* object);
	void RenderPhongLOD(LoDObject* object);

	void SetNoLOD();
	void SetStaticLOD();
	void SetUnpoppingLOD();
	void SetCPNTLOD();
	void SetPhongLOD();

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
