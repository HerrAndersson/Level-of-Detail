#include "stdafx.h"
#include "LevelOfDetail.h"
#include <stdio.h>

/*

Every calculation of distance between the camera and the object (For example when determining the LoD-level) assumes that the object is at (0,0,0). 
If the object should move it needs to hold its own position. The distance would then be calculated using the vector between camPos and objPos

*/

LevelOfDetail::LevelOfDetail(UINT width, UINT height, std::wstring name) :
	DXSample(width, height, name),
	activeTechnique(LoDTechnique::STATIC),
	wireframeModeActive(false),
	freelookCameraActive(false),
	rotation(0, 0, 0),
	objectIndex(0),
	tessellationMinDistance(1.0f),
	tessellationRange(25.0f),
	tessellationFactor(3.0f),
	range(10),
	sphericalCoordDegrees(0, 0),
	goForward(true)
{
	for (int i = 0; i < 1; i++)
	{
		colors[i] = float3((RandomPercent() + 1.0f) / 2.0f, (RandomPercent() + 1.0f) / 2.0f, (RandomPercent() + 1.0f) / 2.0f);
	}

	colors[0] = float3(0.2f, 0.8f, 0.3f);
}

void LevelOfDetail::OnInit()
{
	dx = new DirectXHandler(Win32Application::GetHwnd());
	deviceRef = dx->GetDevice();
	deviceContextRef = dx->GetDeviceContext();

	profiler.Init(deviceRef);

	camera.Init({ 0.0f, 0.0f, 0.0f }, Camera::CameraMode::SCRIPTED);
	camera.SetMoveSpeed(0.3f);
	camera.SetTurnSpeed(XM_PI);

	freelookCamera.Init({ 0.0f, 0.0f, 10.0f }, Camera::CameraMode::MOUSE);
	freelookCamera.SetMoveSpeed(10.0f);
	freelookCamera.SetTurnSpeed(XM_PI);

	projectionMatrix = camera.GetProjectionMatrix(XM_PIDIV2, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	viewMatrix = camera.GetViewMatrix();

	LoadPipelineObjects();
	LoadAssets();

	dx->SetBlendState(DirectXHandler::BlendState::ADDITIVE_ALPHA);
}

void LevelOfDetail::OnDestroy()
{
	delete dx;

	delete defaultVS;
	delete tessellationVS;

	SAFE_RELEASE(defaultPS);
	SAFE_RELEASE(cpntHS);
	SAFE_RELEASE(phongHS);
	SAFE_RELEASE(cpntDS);
	SAFE_RELEASE(phongDS);

	SAFE_RELEASE(samplerWrap);

	SAFE_RELEASE(cbPerObjectVS);
	SAFE_RELEASE(cbPerFrameVS);
	SAFE_RELEASE(cbPerObjectPS);
	SAFE_RELEASE(cbPerFrameDS);
	SAFE_RELEASE(cbPerObjectHS);

	for (auto o : lodObjects)
		delete o;
}

void LevelOfDetail::LoadAssets()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
		throw runtime_error("AssetManager::LoadTexture: Failed in CoInitializeEx. " + GetErrorMessageFromHRESULT(hr));

	LoDObject* object = nullptr;
	
	object = new LoDObject();
	object->texture = AssetManager::LoadTexture(deviceRef, string(TEXTURE_PATH + "metal.jpg"));
	object->models[0] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Truck/truck0.obj"));
	object->models[1] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Truck/truck1.obj"));
	object->models[2] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Truck/truck2.obj"));
	object->models[3] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Truck/truck3.obj"));
	object->models[4] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Truck/truck4.obj"));
	lodObjects.push_back(object);

	object = new LoDObject();
	object->texture = nullptr;
	object->models[0] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Test/cube.obj"));
	object->models[1] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Test/cube.obj"));
	object->models[2] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Test/cube.obj"));
	object->models[3] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Test/cube.obj"));
	object->models[4] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Test/cube.obj"));
	lodObjects.push_back(object);

	object = new LoDObject();
	object->texture = nullptr;
	object->models[0] = AssetManager::LoadModelNoUV(deviceRef, string(MODEL_PATH + "Dragon/dragon0.obj"));
	object->models[1] = AssetManager::LoadModelNoUV(deviceRef, string(MODEL_PATH + "Dragon/dragon1.obj"));
	object->models[2] = AssetManager::LoadModelNoUV(deviceRef, string(MODEL_PATH + "Dragon/dragon2.obj"));
	object->models[3] = AssetManager::LoadModelNoUV(deviceRef, string(MODEL_PATH + "Dragon/dragon3.obj"));
	object->models[4] = AssetManager::LoadModelNoUV(deviceRef, string(MODEL_PATH + "Dragon/dragon4.obj"));
	lodObjects.push_back(object);

	object = new LoDObject();
	object->texture = AssetManager::LoadTexture(deviceRef, string(TEXTURE_PATH + "man.jpg"));
	object->models[0] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Man/man0.obj"));
	object->models[1] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Man/man1.obj"));
	object->models[2] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Man/man2.obj"));
	object->models[3] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Man/man3.obj"));
	object->models[4] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Man/man4.obj"));
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

	//Vertex shaders
	defaultVS = ShaderHandler::CreateVertexShader(deviceRef, L"Shaders/DefaultVS.hlsl", posTexNormInputDesc, numElements, compileFlags);
	tessellationVS = ShaderHandler::CreateVertexShader(deviceRef, L"Shaders/TessellationVS.hlsl", posTexNormInputDesc, numElements, compileFlags);
	//Hull shaders
	cpntHS = ShaderHandler::CreateHullShader(deviceRef, L"Shaders/CpntHS.hlsl", compileFlags);
	phongHS = ShaderHandler::CreateHullShader(deviceRef, L"Shaders/PhongHS.hlsl", compileFlags);
	//Domain shaders
	cpntDS = ShaderHandler::CreateDomainShader(deviceRef, L"Shaders/CpntDS.hlsl", compileFlags);
	phongDS = ShaderHandler::CreateDomainShader(deviceRef, L"Shaders/PhongDS.hlsl", compileFlags);
	//Pixel shaders
	defaultPS = ShaderHandler::CreatePixelShader(deviceRef, L"Shaders/DefaultPS.hlsl", compileFlags);

	//Samplers
	samplerWrap = ShaderHandler::CreateSamplerState(deviceRef, SamplerStates::WRAP);

	//Initialize constant buffers
	HRESULT result;
	D3D11_BUFFER_DESC matrixBufferDesc;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	matrixBufferDesc.ByteWidth = sizeof(CBPerObjectVS);
	result = deviceRef->CreateBuffer(&matrixBufferDesc, NULL, &cbPerObjectVS);
	if (FAILED(result))
		throw std::runtime_error("LevelOfDetail::LoadPipelineObjects: Failed to create cbPerObjectVS. " + GetErrorMessageFromHRESULT(result));

	matrixBufferDesc.ByteWidth = sizeof(CBPerObjectPS);
	result = deviceRef->CreateBuffer(&matrixBufferDesc, NULL, &cbPerObjectPS);
	if (FAILED(result))
		throw std::runtime_error("LevelOfDetail::LoadPipelineObjects: Failed to create cbPerObjectPS. " +GetErrorMessageFromHRESULT(result));

	matrixBufferDesc.ByteWidth = sizeof(CBPerFrame);
	result = deviceRef->CreateBuffer(&matrixBufferDesc, NULL, &cbPerFrameVS);
	if (FAILED(result))
		throw std::runtime_error("LevelOfDetail::LoadPipelineObjects: Failed to create cbPerFrameVS. " + GetErrorMessageFromHRESULT(result));

	matrixBufferDesc.ByteWidth = sizeof(CBPerFrame);
	result = deviceRef->CreateBuffer(&matrixBufferDesc, NULL, &cbPerFrameDS);
	if (FAILED(result))
		throw std::runtime_error("LevelOfDetail::LoadPipelineObjects: Failed to create cbPerFrameDS. " + GetErrorMessageFromHRESULT(result));

	matrixBufferDesc.ByteWidth = sizeof(CBPerObjectHS);
	result = deviceRef->CreateBuffer(&matrixBufferDesc, NULL, &cbPerObjectHS);
	if (FAILED(result))
		throw std::runtime_error("LevelOfDetail::LoadPipelineObjects: Failed to create cbPerObjectHS. " + GetErrorMessageFromHRESULT(result));

	//Set states and shaders
	switch (activeTechnique)
	{
	case LoDTechnique::NO_LOD:
		SetNoLOD();
		break;
	case LoDTechnique::STATIC:
		SetStaticLOD();
		break;
	case LoDTechnique::UNPOPPING:
		SetUnpoppingLOD();
		break;
	case LoDTechnique::CPNT:
		SetCPNTLOD();
		break;
	case LoDTechnique::PHONG:
		SetPhongLOD();
		break;
	default:
		SetNoLOD();
		break;
	}
}

void LevelOfDetail::SetCBPerObject(matrix world, float3 color, float blendFactor, int hasTexture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	//Vertex shader buffers
	CBPerObjectVS* dataPtrVS;
	result = deviceContextRef->Map(cbPerObjectVS, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		throw std::runtime_error("LevelOfDetail::SetCBPerObject: Failed to Map cbPerObject");

	dataPtrVS = static_cast<CBPerObjectVS*>(mappedResource.pData);
	dataPtrVS->world = XMMatrixTranspose(world.ToSIMD());
	deviceContextRef->Unmap(cbPerObjectVS, 0);

	deviceContextRef->VSSetConstantBuffers(1, 1, &cbPerObjectVS);

	//Pixel shader buffers
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	CBPerObjectPS* dataPtrPS;
	result = deviceContextRef->Map(cbPerObjectPS, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		throw std::runtime_error("LevelOfDetail::SetCBPerObject: Failed to Map cbPerObject");

	dataPtrPS = static_cast<CBPerObjectPS*>(mappedResource.pData);
	dataPtrPS->blendFactor = blendFactor;
	dataPtrPS->color = color;
	dataPtrPS->hasTexture = hasTexture;
	deviceContextRef->Unmap(cbPerObjectVS, 0);

	deviceContextRef->PSSetConstantBuffers(0, 1, &cbPerObjectPS);
}

void LevelOfDetail::SetCBPerFrame(matrix view, matrix projection)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	CBPerFrame* dataPtr;

	result = deviceContextRef->Map(cbPerFrameVS, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		throw std::runtime_error("LevelOfDetail::SetCBPerFrame: Failed to Map cbPerFrame");

	dataPtr = static_cast<CBPerFrame*>(mappedResource.pData);
	dataPtr->view = XMMatrixTranspose(view.ToSIMD());
	dataPtr->projection = XMMatrixTranspose(projection.ToSIMD());
	deviceContextRef->Unmap(cbPerFrameVS, 0);

	deviceContextRef->VSSetConstantBuffers(0, 1, &cbPerFrameVS);
}

void LevelOfDetail::SetTessellationCBPerFrame(matrix view, matrix projection)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	CBPerFrame* dataPtr;

	result = deviceContextRef->Map(cbPerFrameDS, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		throw std::runtime_error("LevelOfDetail::SetTessellationCBPerFrame: Failed to Map cbPerFrameDS");

	dataPtr = static_cast<CBPerFrame*>(mappedResource.pData);
	dataPtr->view = XMMatrixTranspose(view.ToSIMD());
	dataPtr->projection = XMMatrixTranspose(projection.ToSIMD());
	deviceContextRef->Unmap(cbPerFrameDS, 0);

	deviceContextRef->DSSetConstantBuffers(0, 1, &cbPerFrameDS);
}
void LevelOfDetail::SetTessellationCBPerObject(matrix world, float3 color, int hasTexture, int lodIndex)
{
	SetCBPerObject(world, color, 1.0f, hasTexture);

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	CBPerObjectHS* dataPtr;

	result = deviceContextRef->Map(cbPerObjectHS, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		throw std::runtime_error("LevelOfDetail::SetTessellationCBPerObject: Failed to Map cbPerObjectHS");

	dataPtr = static_cast<CBPerObjectHS*>(mappedResource.pData);
	dataPtr->cameraPosition = camera.GetPosition();
	dataPtr->tessellationFactor = tessellationFactor;

	XMVECTOR v = XMLoadFloat3(&camera.GetPosition());
	XMVectorSetW(v, 1.0f);

	dataPtr->viewVector = v;
	dataPtr->minDistance = 0;
	dataPtr->range = static_cast<float>(LOD_LEVELS[lodIndex]);

	deviceContextRef->Unmap(cbPerObjectHS, 0);

	deviceContextRef->HSSetConstantBuffers(0, 1, &cbPerObjectHS);
}

//Update frame-based values
void LevelOfDetail::OnUpdate()
{
	timer.Tick(NULL);
	float frameTime = static_cast<float>(timer.GetElapsedSeconds());
	float gameTime = static_cast<float>(timer.GetTotalSeconds());

	float2 difference;
	bool moved = false;
	if (freelookCameraActive)
	{
		mouse.Update();
		moved = mouse.MouseMoved(difference);
		freelookCamera.Update(frameTime, gameTime, moved, difference);
		viewMatrix = freelookCamera.GetViewMatrix();
	}
	else
	{
		viewMatrix = camera.GetViewMatrix();
	}

	camera.Update(frameTime, gameTime, moved, difference);

	LoDObject* object = lodObjects[objectIndex];

	//Decide lod-level. For unpopping it is decided in RenderUnpoppingLOD-function
	float length = camera.GetPosition().Length();
	if (activeTechnique != UNPOPPING)
	{
		if (length < LOD_LEVELS[0])
		{
			object->lodIndex = 0;
		}
		else if (length < LOD_LEVELS[1])
		{
			object->lodIndex = 1;
		}
		else if (length < LOD_LEVELS[2])
		{
			object->lodIndex = 2;
		}
		else if (length < LOD_LEVELS[3])
		{
			object->lodIndex = 3;
		}
		else if (length < LOD_LEVELS[4])
		{
			object->lodIndex = 4;
		}
		else
		{
			object->lodIndex = 4;
		}
	}

	//float increment = 0.0f;
	//sphericalCoordDegrees.x += increment;
	//sphericalCoordDegrees.y += increment;

	if (goForward)
		range -= 0.01f;
	else
		range += 0.01f;

	if (range < 3.0f || range > LOD_LEVELS[4])
		goForward = !goForward;

	float3 newPos;
	newPos.x = range * sin(sphericalCoordDegrees.x) * cos(sphericalCoordDegrees.y);
	newPos.y = range * sin(sphericalCoordDegrees.x) * sin(sphericalCoordDegrees.y);
	newPos.z = range * cos(sphericalCoordDegrees.x);

	camera.SetPosition(newPos);

	//Dragon
	worldMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * XMMatrixTranslation(0.0f, 0.0f, 0.0f);

#if _DEBUG
	string s = string("FPS: " + to_string(fps));
	SetWindowText(Win32Application::GetHwnd(), wstring(s.begin(), s.end()).c_str());
#endif

}

//Render the scene
void LevelOfDetail::OnRender()
{
	dx->BeginScene(1.0f, 1.0f, 1.0f, 1.0f);

	deviceContextRef->Begin(profiler.queryDisjoint);
	deviceContextRef->End(profiler.queryBeginFrame);
	deviceContextRef->Begin(profiler.queryPipelineStatistics);

	LoDObject* object = lodObjects[objectIndex];

	//Decide lod-level. For unpopping it is decided in RenderUnpoppingLOD-function
	if (activeTechnique != UNPOPPING)
	{
		float length = camera.GetPosition().Length();
		if (length < LOD_LEVELS[0])
			object->lodIndex = 0;
		else if (length < LOD_LEVELS[1])
			object->lodIndex = 1;
		else if (length < LOD_LEVELS[2])
			object->lodIndex = 2;
		else if (length < LOD_LEVELS[3])
			object->lodIndex = 3;
		else if (length < LOD_LEVELS[4])
			object->lodIndex = 4;
		else
			object->lodIndex = 4;
	}

	switch (activeTechnique)
	{
	case LoDTechnique::NO_LOD:
		RenderNoLOD(object);
		break;
	case LoDTechnique::STATIC:
		RenderStaticLOD(object);
		break;
	case LoDTechnique::UNPOPPING:
		RenderUnpoppingLOD(object);
		break;
	case LoDTechnique::CPNT:
		RenderCPNTLOD(object);
		break;
	case LoDTechnique::PHONG:
		RenderPhongLOD(object);
		break;
	default:
		RenderNoLOD(object);
		break;
	}

	deviceContextRef->End(profiler.queryRenderObject);

	deviceContextRef->End(profiler.queryEndFrame);
	deviceContextRef->End(profiler.queryDisjoint);
	deviceContextRef->End(profiler.queryPipelineStatistics);

	dx->EndScene();
}

void LevelOfDetail::RenderNoLOD(LoDObject* object)
{
	SetCBPerFrame(viewMatrix, projectionMatrix);

	//Set resources
	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;
	deviceContextRef->IASetVertexBuffers(0, 1, &object->models[0]->vertexBuffer, &vertexSize, &offset);

	int hasTexture = 0;
	if (object->texture)
	{
		deviceContextRef->PSSetShaderResources(0, 1, &object->texture);
		hasTexture = 10;
	}

	//Render
	for (int i = 0; i < 50; i++)
	{
		worldMatrix *= XMMatrixTranslation(i * (colors[i].x * colors[i].y) , 0, i * colors[i].z);
		SetCBPerObject(worldMatrix, colors[i], 1.0f, hasTexture);
		deviceContextRef->Draw(object->models[0]->vertexBufferSize, 0);
	}
}

void LevelOfDetail::RenderStaticLOD(LoDObject* object)
{
	SetCBPerFrame(viewMatrix, projectionMatrix);

	//Set resources
	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;
	deviceContextRef->IASetVertexBuffers(0, 1, &object->models[object->lodIndex]->vertexBuffer, &vertexSize, &offset);

	int hasTexture = 0;
	if (object->texture)
	{
		deviceContextRef->PSSetShaderResources(0, 1, &object->texture);
		hasTexture = 10;
	}

	//Render
	SetCBPerObject(worldMatrix, colors[0], 1.0f, hasTexture);
	deviceContextRef->Draw(object->models[object->lodIndex]->vertexBufferSize, 0);
}

void LevelOfDetail::RenderUnpoppingLOD(LoDObject* object)
{
	SetCBPerFrame(viewMatrix, projectionMatrix);

	if (object->unpopBlendTimerActive)
	{
		if (object->unpopBlendTime > object->unpopBlendLimit)
		{
			object->unpopBlendTimerActive = false;
			dx->SetDepthState(DirectXHandler::DepthState::TEST_WRITE);
		}
		else
		{
			object->unpopBlendTime += static_cast<float>(timer.GetElapsedSeconds());
		}
	}

	//Have a timer that counts up to 1 second. when < 1 second = render lodPreviousIndex and lodIndex. 
	/*Schedule the z-writes and z-tests:
	
	 ___________________________________________
	|	   |	 [0, 0.5] 	  |	    [0.5, 1]	|
	|------|------------------|-----------------|
	|      |   OLD     NEW    |   OLD     NEW   |
	|------|------------------|-----------------|
	|Alpha |    1	    2t    |  2(1-t)    1	|
	|Test  |	e	    e     |    e	   e	|
	|Write |	e	    d     |    d       e	|
	¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯ 

	*/

	float alpha1 = 1.0f;
	float alpha2 = 0.0f;

	bool write1 = false;
	bool write2 = false;

	if (object->unpopBlendTimerActive)
	{
		if (object->unpopBlendTime < object->unpopBlendLimit / 2.0f)
		{
			alpha1 = 1.0f;
			alpha2 = 2.0f * object->unpopBlendTime;
			write1 = true;
			write2 = false;
		}
		else
		{
			alpha1 = 2.0f * (1.0f - object->unpopBlendTime);
			alpha2 = 1.0f;
			write1 = false;
			write2 = true;
		}

		UINT32 vertexSize = sizeof(Vertex);
		UINT32 offset = 0;

		//----------------------------- Old LoD -----------------------------
		//Set resources for OLD
		if (write1)
			dx->SetDepthState(DirectXHandler::DepthState::TEST_WRITE);
		else
			dx->SetDepthState(DirectXHandler::DepthState::TEST_NO_WRITE);

		deviceContextRef->IASetVertexBuffers(0, 1, &object->models[object->lodIndexPrevious]->vertexBuffer, &vertexSize, &offset);

		int hasTexture = 0;
		if (object->texture)
		{
			deviceContextRef->PSSetShaderResources(0, 1, &object->texture);
			hasTexture = 10;
		}

		//Render OLD
		SetCBPerObject(worldMatrix, colors[0], alpha1, hasTexture);
		deviceContextRef->Draw(object->models[object->lodIndexPrevious]->vertexBufferSize, 0);

		//----------------------------- New LoD -----------------------------
		//Set resources for NEW
		if (write2)
			dx->SetDepthState(DirectXHandler::DepthState::TEST_WRITE);
		else
			dx->SetDepthState(DirectXHandler::DepthState::TEST_NO_WRITE);

		deviceContextRef->IASetVertexBuffers(0, 1, &object->models[object->lodIndex]->vertexBuffer, &vertexSize, &offset);
		hasTexture = 0;
		if (object->texture)
		{
			deviceContextRef->PSSetShaderResources(0, 1, &object->texture);
			hasTexture = 10;
		}

		//Render NEW
		SetCBPerObject(worldMatrix, colors[0], alpha2, hasTexture);
		deviceContextRef->Draw(object->models[object->lodIndex]->vertexBufferSize, 0);
	}
	else
	{
		//Decide lod-level of objects
		float3 camPos = camera.GetPosition();
		float length = camPos.Length();

		bool switchLoD = false;
		if (length < LOD_LEVELS[0])
		{
			object->lodIndexPrevious = object->lodIndex;
			object->lodIndex = 0;
		}
		else if (length < LOD_LEVELS[1])
		{
			object->lodIndexPrevious = object->lodIndex;
			object->lodIndex = 1;
		}
		else if (length < LOD_LEVELS[2])
		{
			object->lodIndexPrevious = object->lodIndex;
			object->lodIndex = 2;
		}
		else if (length < LOD_LEVELS[3])
		{
			object->lodIndexPrevious = object->lodIndex;
			object->lodIndex = 3;
		}
		else if (length < LOD_LEVELS[4])
		{
			object->lodIndexPrevious = object->lodIndex;
			object->lodIndex = 4;
		}
		else
		{
			object->lodIndexPrevious = object->lodIndex;
			object->lodIndex = 4;
		}

		if (object->lodIndex != object->lodIndexPrevious)
			switchLoD = true;

		if (switchLoD)
		{
			switchLoD = false;
			object->unpopBlendLimit = BLEND_TIME;
			object->unpopBlendTime = 0.0f;
			object->unpopBlendTimerActive = true;
		}

		//Set resources for current index
		UINT32 vertexSize = sizeof(Vertex);
		UINT32 offset = 0;
		deviceContextRef->IASetVertexBuffers(0, 1, &object->models[object->lodIndex]->vertexBuffer, &vertexSize, &offset);
		int hasTexture = 0;
		if (object->texture)
		{
			deviceContextRef->PSSetShaderResources(0, 1, &object->texture);
			hasTexture = 10;
		}

		//Render current index
		SetCBPerObject(worldMatrix, colors[0], 1.0f, hasTexture);
		deviceContextRef->Draw(object->models[object->lodIndex]->vertexBufferSize, 0);
	}
}

void LevelOfDetail::RenderCPNTLOD(LoDObject* object)
{
	SetTessellationCBPerFrame(viewMatrix, projectionMatrix);

	//Set resources
	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;
	deviceContextRef->IASetVertexBuffers(0, 1, &object->models[object->lodIndex]->vertexBuffer, &vertexSize, &offset);
	int hasTexture = 0;
	if (object->texture)
	{
		deviceContextRef->PSSetShaderResources(0, 1, &object->texture);
		hasTexture = 10;
	}

	//Render
	SetTessellationCBPerObject(worldMatrix, colors[0], hasTexture, object->lodIndex);
	deviceContextRef->Draw(object->models[object->lodIndex]->vertexBufferSize, 0);
}

void LevelOfDetail::RenderPhongLOD(LoDObject* object)
{
	SetTessellationCBPerFrame(viewMatrix, projectionMatrix);

	//Set resources
	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;
	deviceContextRef->IASetVertexBuffers(0, 1, &object->models[object->lodIndex]->vertexBuffer, &vertexSize, &offset);
	int hasTexture = 0;
	if (object->texture)
	{
		deviceContextRef->PSSetShaderResources(0, 1, &object->texture);
		hasTexture = 10;
	}

	//Render
	SetTessellationCBPerObject(worldMatrix, colors[0], hasTexture, object->lodIndex);
	deviceContextRef->Draw(object->models[object->lodIndex]->vertexBufferSize, 0);
}

void LevelOfDetail::OnKeyDown(UINT8 key)
{
	camera.OnKeyDown(key);
	freelookCamera.OnKeyDown(key);

	switch (key)
	{
		case '0':
			SetNoLOD();
			break;
		case '1':
			SetStaticLOD();
			break;
		case '2':
			SetUnpoppingLOD();
			break;
		case '3':
			SetCPNTLOD();
			break;
		case '4':
			SetPhongLOD();
			break;
		case VK_TAB:
			freelookCameraActive = !freelookCameraActive;
			break;
		case VK_SHIFT:
		{
			wireframeModeActive = !wireframeModeActive;

			if (wireframeModeActive)
				dx->SetRasterState(DirectXHandler::RasterState::WIREFRAME);
			else
				dx->SetRasterState(DirectXHandler::RasterState::CULL_BACK);

			break;
		}
		case 'Z':
			objectIndex = 0;
			break;
		case 'X':
			objectIndex = 1;
			break;
		case 'C':
			objectIndex = 2;
			break;
		case 'V':
			objectIndex = 3;
			break;
		default:
			break;
	}
}

void LevelOfDetail::OnKeyUp(UINT8 key)
{
	camera.OnKeyUp(key);
	freelookCamera.OnKeyUp(key);

	switch (key)
	{
	case 'N':
		tessellationFactor-=0.1f;
		break;
	case 'M':
		tessellationFactor+=0.1f;
		break;
	default:
		break;
	}
}

void LevelOfDetail::SetNoLOD()
{
	activeTechnique = LoDTechnique::NO_LOD;

	deviceContextRef->IASetInputLayout(defaultVS->inputLayout);
	deviceContextRef->VSSetShader(defaultVS->vertexShader, nullptr, 0);
	deviceContextRef->HSSetShader(nullptr, nullptr, 0);
	deviceContextRef->DSSetShader(nullptr, nullptr, 0);
	deviceContextRef->PSSetShader(defaultPS, nullptr, 0);

	dx->SetDepthState(DirectXHandler::DepthState::TEST_WRITE);

	deviceContextRef->PSSetSamplers(0, 1, &samplerWrap);
	deviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void LevelOfDetail::SetStaticLOD()
{
	activeTechnique = LoDTechnique::STATIC;

	deviceContextRef->IASetInputLayout(defaultVS->inputLayout);
	deviceContextRef->VSSetShader(defaultVS->vertexShader, nullptr, 0);
	deviceContextRef->HSSetShader(nullptr, nullptr, 0);
	deviceContextRef->DSSetShader(nullptr, nullptr, 0);
	deviceContextRef->PSSetShader(defaultPS, nullptr, 0);

	dx->SetDepthState(DirectXHandler::DepthState::TEST_WRITE);

	deviceContextRef->PSSetSamplers(0, 1, &samplerWrap);
	deviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void LevelOfDetail::SetUnpoppingLOD()
{
	activeTechnique = LoDTechnique::UNPOPPING;

	dx->SetBlendState(Renderer::DirectXHandler::BlendState::ADDITIVE_ALPHA);

	deviceContextRef->IASetInputLayout(defaultVS->inputLayout);
	deviceContextRef->VSSetShader(defaultVS->vertexShader, nullptr, 0);
	deviceContextRef->HSSetShader(nullptr, nullptr, 0);
	deviceContextRef->DSSetShader(nullptr, nullptr, 0);
	deviceContextRef->PSSetShader(defaultPS, nullptr, 0);

	dx->SetDepthState(DirectXHandler::DepthState::TEST_WRITE);

	deviceContextRef->PSSetSamplers(0, 1, &samplerWrap);
	deviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void LevelOfDetail::SetCPNTLOD()
{
	activeTechnique = LoDTechnique::CPNT;

	deviceContextRef->IASetInputLayout(tessellationVS->inputLayout);
	deviceContextRef->VSSetShader(tessellationVS->vertexShader, nullptr, 0);
	deviceContextRef->HSSetShader(cpntHS, nullptr, 0);
	deviceContextRef->DSSetShader(cpntDS, nullptr, 0);
	deviceContextRef->PSSetShader(defaultPS, nullptr, 0);

	dx->SetDepthState(DirectXHandler::DepthState::TEST_WRITE);

	deviceContextRef->PSSetSamplers(0, 1, &samplerWrap);
	deviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
}

void LevelOfDetail::SetPhongLOD()
{
	activeTechnique = LoDTechnique::PHONG;

	deviceContextRef->IASetInputLayout(tessellationVS->inputLayout);
	deviceContextRef->VSSetShader(tessellationVS->vertexShader, nullptr, 0);
	deviceContextRef->HSSetShader(phongHS, nullptr, 0);
	deviceContextRef->DSSetShader(phongDS, nullptr, 0);
	deviceContextRef->PSSetShader(defaultPS, nullptr, 0);

	dx->SetDepthState(DirectXHandler::DepthState::TEST_WRITE);

	deviceContextRef->PSSetSamplers(0, 1, &samplerWrap);
	deviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
}

// Random percent value, from -1 to 1.
float LevelOfDetail::RandomPercent()
{
	float ret = static_cast<float>((rand() % 10000) - 5000);
	return ret / 5000.0f;
}