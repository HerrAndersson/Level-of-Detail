#include "stdafx.h"
#include "LevelOfDetail.h"

LevelOfDetail::LevelOfDetail(UINT width, UINT height, std::wstring name) :
	DXSample(width, height, name)
{
}

void LevelOfDetail::OnInit()
{
	dx = new DirectXHandler(Win32Application::GetHwnd());

	camera.Init({ 0.0f, 0.0f, 1500.0f });
	camera.SetMoveSpeed(250.0f);

	projectionMatrix = camera.GetProjectionMatrix(XM_PIDIV2, SCREEN_WIDTH / SCREEN_HEIGHT);

	LoadShaders();
	LoadAssets();
}

void LevelOfDetail::LoadAssets()
{
	am.LoadObject(dx->GetDevice(), MODEL_PATH + "cube.obj", TEXTURE_PATH + "sand.png");
}

// Load the sample assets.
void LevelOfDetail::LoadShaders()
{

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_STRICTNESS;
#else
	UINT compileFlags = 0;
#endif;

	////////////////////////////////////////////////////////////// Create Shaders ///////////////////////////////////////////////////////////////
	D3D11_INPUT_ELEMENT_DESC posTexNormInputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	int numElements = sizeof(posTexNormInputDesc) / sizeof(posTexNormInputDesc[0]);

	defaultVS = ShaderHandler::CreateVertexShader(dx->GetDevice(), L"Shaders/ModelVS.hlsl", posTexNormInputDesc, numElements, compileFlags);
	defaultPS = ShaderHandler::CreatePixelShader(dx->GetDevice(), L"Shaders/ModelPS.hlsl", compileFlags);
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
	delete defaultVS;
}

void LevelOfDetail::OnKeyDown(UINT8 key)
{
	camera.OnKeyDown(key);
}

void LevelOfDetail::OnKeyUp(UINT8 key)
{
	camera.OnKeyUp(key);
}