#include "stdafx.h"
#include "LevelOfDetail.h"

LevelOfDetail::LevelOfDetail(UINT width, UINT height, std::wstring name) :
	DXSample(width, height, name)
{
}

void LevelOfDetail::OnInit()
{
	camera.Init({ 0.0f, 0.0f, 1500.0f });
	camera.SetMoveSpeed(250.0f);

	projectionMatrix = camera.GetProjectionMatrix(XM_PIDIV2, SCREEN_WIDTH / SCREEN_HEIGHT);

	LoadPipeline();
	LoadAssets();
}

// Load the rendering pipeline dependencies
void LevelOfDetail::LoadPipeline()
{
	dx = new DirectXHandler(Win32Application::GetHwnd());
}

// Load the sample assets.
void LevelOfDetail::LoadAssets()
{
	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif;

	std::wstring shaderPath = L"Shaders/";

	ThrowIfFailed(D3DCompileFromFile((shaderPath+L"ModelVS.hlsl").c_str(), nullptr, nullptr, "main", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
	ThrowIfFailed(D3DCompileFromFile((shaderPath + L"ModelPS.hlsl").c_str(), nullptr, nullptr, "main", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

	



//	// Create the pipeline states, which includes compiling and loading shaders.
//	{
//		ComPtr<ID3DBlob> vertexShader;
//		ComPtr<ID3DBlob> geometryShader;
//		ComPtr<ID3DBlob> pixelShader;
//		ComPtr<ID3DBlob> computeShader;
//

//
//		// Load and compile shaders.
//		ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"ParticleDraw.hlsl").c_str(), nullptr, nullptr, "VSParticleDraw", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
//		ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"ParticleDraw.hlsl").c_str(), nullptr, nullptr, "GSParticleDraw", "gs_5_0", compileFlags, 0, &geometryShader, nullptr));
//		ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"ParticleDraw.hlsl").c_str(), nullptr, nullptr, "PSParticleDraw", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));
//		ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"NBodyGravityCS.hlsl").c_str(), nullptr, nullptr, "CSMain", "cs_5_0", compileFlags, 0, &computeShader, nullptr));
//
//		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
//		{
//			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//		};
}

// Random percent value, from -1 to 1.
float LevelOfDetail::RandomPercent()
{
	float ret = static_cast<float>((rand() % 10000) - 5000);
	return ret / 5000.0f;
}

//Update frame-based values
void LevelOfDetail::OnUpdate()
{
	timer.Tick(NULL);
	camera.Update(static_cast<float>(timer.GetElapsedSeconds()));
}

//Render the scene
void LevelOfDetail::OnRender()
{
	dx->BeginScene(0, 0.3f, 0.3f, 1);

	XMMATRIX world = XMMatrixTranslation(0, 0, 0);
	XMMATRIX view = camera.GetViewMatrix();
	XMMATRIX projection = projectionMatrix.ToSIMD();



	dx->EndScene();
}

void LevelOfDetail::OnDestroy()
{
	delete dx;
}

void LevelOfDetail::OnKeyDown(UINT8 key)
{
	camera.OnKeyDown(key);
}

void LevelOfDetail::OnKeyUp(UINT8 key)
{
	camera.OnKeyUp(key);
}