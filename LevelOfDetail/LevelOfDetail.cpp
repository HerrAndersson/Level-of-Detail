#include "stdafx.h"
#include "LevelOfDetail.h"

LevelOfDetail::LevelOfDetail(UINT width, UINT height, std::wstring name) :
	DXSample(width, height, name)
{
}

void LevelOfDetail::OnInit()
{
	dx = new DirectXHandler(Win32Application::GetHwnd());
	deviceRef = dx->GetDevice();
	deviceContextRef = dx->GetDeviceContext();

	camera.Init({ 0.0f, 0.0f, 1500.0f });
	camera.SetMoveSpeed(250.0f);

	projectionMatrix = camera.GetProjectionMatrix(XM_PIDIV2, SCREEN_WIDTH / SCREEN_HEIGHT);

	LoadPipelineObjects();
	LoadAssets();
}

void LevelOfDetail::LoadAssets()
{
	am.LoadObject(deviceRef, MODEL_PATH + "cube.obj", TEXTURE_PATH + "sand.png");
}

void LevelOfDetail::LoadPipelineObjects()
{

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools
	UINT compileFlags = 0; //D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_STRICTNESS;
#else
	UINT compileFlags = 0;
#endif;



	//Temp -----------------------
	float left = -10.0f;
	float right = 10.0f;
	float top = 10.0f;
	float bottom = -10.0f;

	SimpleVertex quad[] =
	{ 
		XMFLOAT3(left, top, -10.0f),
		XMFLOAT3(right, bottom, -10.0f),
		XMFLOAT3(left, bottom, -10.0f),
		XMFLOAT3(left, top, 10.0f),
		XMFLOAT3(right, top, 10.0f),
		XMFLOAT3(right, bottom, 10.0f)
	};

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(SimpleVertex) * 6;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = quad;
	HRESULT result = deviceRef->CreateBuffer(&bufferDesc, &data, &vertexBuffer);
	//-----------------------


	//Create shaders
	D3D11_INPUT_ELEMENT_DESC posTexNormInputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	int numElements = sizeof(posTexNormInputDesc) / sizeof(posTexNormInputDesc[0]);



	defaultVS = ShaderHandler::CreateVertexShader(deviceRef, L"Shaders/ModelVS.hlsl", posTexNormInputDesc, numElements, compileFlags);
	defaultPS = ShaderHandler::CreatePixelShader(deviceRef, L"Shaders/ModelPS.hlsl", compileFlags);

	samplerWrap = ShaderHandler::CreateSamplerState(deviceRef, SamplerStates::WRAP);


	//Initialize constant buffers
	//HRESULT result;
	D3D11_BUFFER_DESC matrixBufferDesc;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	matrixBufferDesc.ByteWidth = sizeof(CBPerObject);
	result = deviceRef->CreateBuffer(&matrixBufferDesc, NULL, &cbPerObject);
	if (FAILED(result))
		throw std::runtime_error("LevelOfDetail::LoadPipelineObjects: Failed to create cbPerObject");

	matrixBufferDesc.ByteWidth = sizeof(CBPerFrame);
	result = deviceRef->CreateBuffer(&matrixBufferDesc, NULL, &cbPerFrame);
	if (FAILED(result))
		throw std::runtime_error("LevelOfDetail::LoadPipelineObjects: Failed to create cbPerFrame");

	//Set initial states and shaders
	deviceContextRef->IASetInputLayout(defaultVS->inputLayout);
	deviceContextRef->VSSetShader(defaultVS->vertexShader, nullptr, 0);
	deviceContextRef->PSSetShader(defaultPS.Get(), nullptr, 0);
	deviceContextRef->PSSetSamplers(0, 1, &samplerWrap);
	deviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void LevelOfDetail::SetCBPerObject(matrix4x4 world)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CBPerObject* dataPtr;

	result = deviceContextRef->Map(cbPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		throw std::runtime_error("LevelOfDetail::SetCBPerObject: Failed to Map cbPerObject");

	dataPtr = static_cast<CBPerObject*>(mappedResource.pData);
	dataPtr->world = XMMatrixTranspose(world.ToSIMD());
	deviceContextRef->Unmap(cbPerObject, 0);

	deviceContextRef->VSSetConstantBuffers(1, 1, &cbPerObject);
}

void LevelOfDetail::SetCBPerFrame(matrix4x4 view, matrix4x4 projection)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CBPerFrame* dataPtr;

	result = deviceContextRef->Map(cbPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		throw std::runtime_error("LevelOfDetail::SetCBPerFrame: Failed to Map cbPerFrame");

	dataPtr = static_cast<CBPerFrame*>(mappedResource.pData);
	dataPtr->view = XMMatrixTranspose(view.ToSIMD());
	dataPtr->projection = XMMatrixTranspose(projection.ToSIMD());
	deviceContextRef->Unmap(cbPerFrame, 0);

	deviceContextRef->VSSetConstantBuffers(0, 1, &cbPerFrame);
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
	dx->BeginScene(0.012f, 0.1f, 0.01f, 1);

	matrix4x4 view = camera.GetViewMatrix();

	SetCBPerFrame(view, projectionMatrix);

	//RenderObject* object = am.GetRenderObject(0);

	UINT32 vertexSize = sizeof(SimpleVertex);
	UINT32 offset = 0;
	deviceContextRef->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);

	deviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContextRef->IASetInputLayout(defaultVS->inputLayout);

	deviceContextRef->VSSetShader(defaultVS->vertexShader, nullptr, 0);
	deviceContextRef->PSSetShader(defaultPS.Get(), nullptr, 0);
	deviceContextRef->PSSetSamplers(0, 1, &samplerWrap);

	for (int i = 0; i < 100; i++)
	{
		matrix4x4 world = XMMatrixTranslation((rand() % 200) - 100, (rand() % 200) - 100, (rand() % 200) - 100);
		SetCBPerObject(world);
		deviceContextRef->Draw(6, 0);
	}

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

// Random percent value, from -1 to 1.
float LevelOfDetail::RandomPercent()
{
	float ret = static_cast<float>((rand() % 10000) - 5000);
	return ret / 5000.0f;
}