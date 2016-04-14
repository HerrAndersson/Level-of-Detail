#include "stdafx.h"
#include "LevelOfDetail.h"

LevelOfDetail::LevelOfDetail(UINT width, UINT height, std::wstring name) :
	DXSample(width, height, name)
{
	for (int i = 0; i < 100; i++)
	{
		colors[i] = float3((RandomPercent() + 1) / 2, (RandomPercent() + 1) / 2, (RandomPercent() + 1) / 2);
	}
}

void LevelOfDetail::OnInit()
{
	dx = new DirectXHandler(Win32Application::GetHwnd());
	deviceRef = dx->GetDevice();
	deviceContextRef = dx->GetDeviceContext();

	camera.Init({ 0.0f, 0.0f, -10.0f }, Camera::CameraMode::SCRIPTED);
	camera.SetMoveSpeed(10.0f);
	camera.SetTurnSpeed(XM_PI);

	projectionMatrix = camera.GetProjectionMatrix(XM_PIDIV2, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);

	LoadPipelineObjects();
	LoadAssets();

	//dx->SetRasterState(Renderer::DirectXHandler::RasterState::WIREFRAME);
	dx->SetBlendState(Renderer::DirectXHandler::BlendState::DISABLED);
}

void LevelOfDetail::OnDestroy()
{
	delete dx;
	delete defaultVS;

	SAFE_RELEASE(defaultPS);
	SAFE_RELEASE(samplerWrap);
	SAFE_RELEASE(cbPerObject);
	SAFE_RELEASE(cbPerFrame);

	for (auto o : lodObjects)
		delete o;
}

void LevelOfDetail::LoadAssets()
{
	LoDObject* object = new LoDObject();

	object->texture = AssetManager::LoadTexture(deviceRef, string(TEXTURE_PATH + "sand.png"));
	object->models[0] = AssetManager::LoadModelNoUV(deviceRef, string(MODEL_PATH + "bunny0.obj"));
	object->models[1] = AssetManager::LoadModelNoUV(deviceRef, string(MODEL_PATH + "bunny1.obj"));
	object->models[2] = AssetManager::LoadModelNoUV(deviceRef, string(MODEL_PATH + "bunny2.obj"));

	lodObjects.push_back(object);
}

void LevelOfDetail::LoadPipelineObjects()
{

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_STRICTNESS;
#else
	UINT compileFlags = 0;
#endif;

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
	HRESULT result;
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
	deviceContextRef->PSSetShader(defaultPS, nullptr, 0);
	deviceContextRef->PSSetSamplers(0, 1, &samplerWrap);
	deviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void LevelOfDetail::SetCBPerObject(matrix world, float3 color, float blendFactor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CBPerObject* dataPtr;

	result = deviceContextRef->Map(cbPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		throw std::runtime_error("LevelOfDetail::SetCBPerObject: Failed to Map cbPerObject");

	dataPtr = static_cast<CBPerObject*>(mappedResource.pData);
	dataPtr->world = XMMatrixTranspose(world.ToSIMD());
	dataPtr->color = color;
	dataPtr->blendFactor = blendFactor;
	deviceContextRef->Unmap(cbPerObject, 0);

	deviceContextRef->VSSetConstantBuffers(1, 1, &cbPerObject);
}

void LevelOfDetail::SetCBPerFrame(matrix view, matrix projection)
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

	float2 difference;
	bool moved = false;
	if (camera.GetCameraMode() == Camera::CameraMode::MOUSE)
	{
		UpdateMouse();
		moved = mouse.MouseMoved(difference);
	}

	camera.Update(static_cast<float>(timer.GetElapsedSeconds()), static_cast<float>(timer.GetTotalSeconds()), moved, difference);

#if _DEBUG
	string s = string("FPS: " + to_string(timer.GetFramesPerSecond()));
	SetWindowText(Win32Application::GetHwnd(), wstring(s.begin(), s.end()).c_str());
#endif

}

void LevelOfDetail::UpdateMouse()
{
	mouse.Update();
}

//Render the scene
void LevelOfDetail::OnRender()
{
	RenderStaticLOD();
}

void LevelOfDetail::RenderNormal()
{
	dx->BeginScene(0.0f, 0.75f, 1.0f, 1.0f);
	matrix view = camera.GetViewMatrix();
	SetCBPerFrame(view, projectionMatrix);

	LoDObject* object = lodObjects[0];

	//Set resources
	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;
	deviceContextRef->IASetVertexBuffers(0, 1, &object->models[0]->vertexBuffer, &vertexSize, &offset);
	if (object->texture)
		deviceContextRef->PSSetShaderResources(0, 1, &object->texture);

	//Render
	for (int i = 0; i < 50; i++)
	{
		matrix world = XMMatrixScaling(5.0f, 5.0f, 5.0f) * XMMatrixTranslation(-25.0f + i*4.0f, 0, float(int(50.0f * colors[i].y)%(int)(colors[i].x*100.0f + 1.0f)));
		SetCBPerObject(world, colors[i], 1.0f);
		deviceContextRef->Draw(object->models[0]->vertexBufferSize, 0);
	}

	dx->EndScene();
}

void LevelOfDetail::RenderStaticLOD()
{
	dx->BeginScene(0.0f, 0.75f, 1.0f, 1.0f);
	matrix view = camera.GetViewMatrix();
	SetCBPerFrame(view, projectionMatrix);

	LoDObject* object = lodObjects[0];

	//Decide lod-level
	float3 camPos = camera.GetPosition();
	float length = camPos.Length();
	int index = 0;
	if (length > LOD_LEVELS[1])
		index = 2;
	else if (length < LOD_LEVELS[0])
		index = 0;
	else
		index = 1;

	//Set resources
	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;
	deviceContextRef->IASetVertexBuffers(0, 1, &object->models[index]->vertexBuffer, &vertexSize, &offset);
	if (object->texture)
		deviceContextRef->PSSetShaderResources(0, 1, &object->texture);

	//Render
	matrix world = XMMatrixScaling(5.0f, 5.0f, 5.0f);
	SetCBPerObject(world, colors[0], 1.0f);
	deviceContextRef->Draw(object->models[index]->vertexBufferSize, 0);

	dx->EndScene();
}

void LevelOfDetail::RenderUnpopLOD()
{
	dx->BeginScene(0.0f, 0.75f, 1.0f, 1.0f);
	matrix view = camera.GetViewMatrix();
	SetCBPerFrame(view, projectionMatrix);

	LoDObject* object = lodObjects[0];

	//Decide lod-level
	float3 camPos = camera.GetPosition();
	float length = camPos.Length();
	int index = 0;
	if (length > LOD_LEVELS[1])
		index = 2;
	else if (length < LOD_LEVELS[0])
		index = 0;
	else
		index = 1;

	//Set resources
	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;
	deviceContextRef->IASetVertexBuffers(0, 1, &object->models[index]->vertexBuffer, &vertexSize, &offset);
	if (object->texture)
		deviceContextRef->PSSetShaderResources(0, 1, &object->texture);

	//Render
	matrix world = XMMatrixScaling(5.0f, 5.0f, 5.0f);
	SetCBPerObject(world, colors[0], 1.0f);
	deviceContextRef->Draw(object->models[index]->vertexBufferSize, 0);

	float percentage = 0;
	if (length > LOD_BLEND_LEVELS[index].low && length < LOD_BLEND_LEVELS[index].high)
	{
		percentage = (length - LOD_BLEND_LEVELS[index].low) / (LOD_BLEND_LEVELS[index].high - LOD_BLEND_LEVELS[index].low);
		index++;
	}

	dx->EndScene();
}

void LevelOfDetail::RenderCPNTLOD()
{

}

void LevelOfDetail::RenderPhongLOD()
{

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