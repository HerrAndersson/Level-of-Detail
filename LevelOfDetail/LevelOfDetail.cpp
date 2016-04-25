#include "stdafx.h"
#include "LevelOfDetail.h"

/*

Every calculation of distance between the camera and the object (For example when determining the LoD-level) assumes that the object is at (0,0,0). 
If the object should move it needs to hold its own position. The distance would then be calculated using the vector between camPos and objPos

*/

LevelOfDetail::LevelOfDetail(UINT width, UINT height, std::wstring name) :
	DXSample(width, height, name),
	activeTechnique(LoDTechnique::UNPOPPING),
	wireframeModeActive(false),
	freelookCameraActive(false),
	rotation(0,0,0),
	objectIndex(0),
	tessellationMinDistance(1.0f),
	tessellationRange(25.0f),
	tessellationFactor(5.0f)
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
	camera.SetMoveSpeed(0.3f);
	camera.SetTurnSpeed(XM_PI);
	freelookCamera.Init({ 0.0f, 0.0f, -10.0f }, Camera::CameraMode::MOUSE);
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

	LoDObject* bunny = new LoDObject();
	bunny->texture = AssetManager::LoadTexture(deviceRef, string(TEXTURE_PATH + "sand.png"));
	bunny->models[0] = AssetManager::LoadModelNoUV(deviceRef, string(MODEL_PATH + "Bunny/bunny0.obj"));

	bunny->models[2] = AssetManager::LoadModelNoUV(deviceRef, string(MODEL_PATH + "Bunny/bunny2.obj"));

	bunny->models[4] = AssetManager::LoadModelNoUV(deviceRef, string(MODEL_PATH + "Bunny/bunny4.obj"));
	lodObjects.push_back(bunny);

	LoDObject* bunny = new LoDObject();
	bunny->texture = AssetManager::LoadTexture(deviceRef, string(TEXTURE_PATH + "sand.png"));
	bunny->models[0] = AssetManager::LoadModelNoUV(deviceRef, string(MODEL_PATH + "Dragon/dragon0.obj"));
	bunny->models[1] = AssetManager::LoadModelNoUV(deviceRef, string(MODEL_PATH + "Dragon/dragon1.obj"));
	bunny->models[2] = AssetManager::LoadModelNoUV(deviceRef, string(MODEL_PATH + "Dragon/dragon2.obj"));
	lodObjects.push_back(bunny);

	LoDObject* cylinder = new LoDObject();
	cylinder->texture = AssetManager::LoadTexture(deviceRef, string(TEXTURE_PATH + "sand.png"));
	cylinder->models[0] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Test/cylinder.obj"));
	cylinder->models[1] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Test/cylinder.obj"));
	cylinder->models[2] = AssetManager::LoadModel(deviceRef, string(MODEL_PATH + "Test/cylinder.obj"));
	lodObjects.push_back(cylinder);
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

void LevelOfDetail::SetCBPerObject(matrix world, float3 color, float blendFactor)
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
void LevelOfDetail::SetTessellationCBPerObject(matrix world, float3 color)
{
	SetCBPerObject(world, color, 1.0f);

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
	dataPtr->minDistance = tessellationMinDistance;
	dataPtr->range = tessellationRange;
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

#if _DEBUG
	string s = string("FPS: " + to_string(timer.GetFramesPerSecond()));
	SetWindowText(Win32Application::GetHwnd(), wstring(s.begin(), s.end()).c_str());
#endif

}

//Render the scene
void LevelOfDetail::OnRender()
{
	LoDObject* object = lodObjects[objectIndex];

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

	//float increment = XM_2PI / 360.0f;
	//if (rotation.x < XM_PI && rotation.y >= XM_PI)
	//{
	//	rotation.x += increment;
	//	rotation.y = 0;
	//}
	//else if (rotation.y < XM_PI && rotation.z >= XM_PI)
	//{
	//	rotation.y += increment;
	//	rotation.z = 0;
	//}
	//else if (rotation.z < XM_PI)
	//{
	//	rotation.z += increment;
	//}

	//dx->SaveBackBufferToFile(StrToWStr("Images/img" + to_string(timer.GetFrameCount()) + ".png"));

	worldMatrix = XMMatrixScaling(5.0f, 5.0f, 5.0f) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
}

void LevelOfDetail::RenderNoLOD(LoDObject* object)
{
	dx->BeginScene(0.0f, 0.75f, 1.0f, 1.0f);
	SetCBPerFrame(viewMatrix, projectionMatrix);

	//Set resources
	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;
	deviceContextRef->IASetVertexBuffers(0, 1, &object->models[0]->vertexBuffer, &vertexSize, &offset);
	if (object->texture)
		deviceContextRef->PSSetShaderResources(0, 1, &object->texture);

	//Render
	for (int i = 0; i < 50; i++)
	{
		worldMatrix *= XMMatrixTranslation(i * (colors[i].x * colors[i].y) , 0, i * colors[i].z);
		SetCBPerObject(worldMatrix, colors[i], 1.0f);
		deviceContextRef->Draw(object->models[0]->vertexBufferSize, 0);
	}

	dx->EndScene();
}

void LevelOfDetail::RenderStaticLOD(LoDObject* object)
{
	dx->BeginScene(0.0f, 0.75f, 1.0f, 1.0f);
	SetCBPerFrame(viewMatrix, projectionMatrix);

	//Decide lod-level
	float3 camPos = camera.GetPosition();
	float length = camPos.Length();

	if (length > LOD_LEVELS[1])
		object->lodIndex = 2;
	else if (length < LOD_LEVELS[0])
		object->lodIndex = 0;
	else
		object->lodIndex = 1;

	//Set resources
	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;
	deviceContextRef->IASetVertexBuffers(0, 1, &object->models[object->lodIndex]->vertexBuffer, &vertexSize, &offset);
	if (object->texture)
		deviceContextRef->PSSetShaderResources(0, 1, &object->texture);

	//Render
	SetCBPerObject(worldMatrix, colors[0], 1.0f);
	deviceContextRef->Draw(object->models[object->lodIndex]->vertexBufferSize, 0);

	dx->EndScene();
}

void LevelOfDetail::RenderUnpoppingLOD(LoDObject* object)
{
	dx->BeginScene(0.0f, 0.75f, 1.0f, 1.0f);
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
	ŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻŻ 

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
		if (object->texture)
			deviceContextRef->PSSetShaderResources(0, 1, &object->texture);

		//Render OLD
		SetCBPerObject(worldMatrix, colors[0], alpha1);
		deviceContextRef->Draw(object->models[object->lodIndexPrevious]->vertexBufferSize, 0);

		//----------------------------- New LoD -----------------------------
		//Set resources for NEW
		if (write2)
			dx->SetDepthState(DirectXHandler::DepthState::TEST_WRITE);
		else
			dx->SetDepthState(DirectXHandler::DepthState::TEST_NO_WRITE);

		deviceContextRef->IASetVertexBuffers(0, 1, &object->models[object->lodIndex]->vertexBuffer, &vertexSize, &offset);
		if (object->texture)
			deviceContextRef->PSSetShaderResources(0, 1, &object->texture);

		//Render NEW
		SetCBPerObject(worldMatrix, colors[0], alpha2);
		deviceContextRef->Draw(object->models[object->lodIndex]->vertexBufferSize, 0);
	}
	else
	{
		//Decide lod-level of objects
		float3 camPos = camera.GetPosition();
		float length = camPos.Length();

		bool switchLoD = false;
		if (length > LOD_LEVELS[1])
		{
			object->lodIndexPrevious = object->lodIndex;
			object->lodIndex = 2;
		}
		else if (length < LOD_LEVELS[0])
		{
			object->lodIndexPrevious = object->lodIndex;
			object->lodIndex = 0;
		}
		else
		{
			object->lodIndexPrevious = object->lodIndex;
			object->lodIndex = 1;
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
		if (object->texture)
			deviceContextRef->PSSetShaderResources(0, 1, &object->texture);

		//Render current index
		SetCBPerObject(worldMatrix, colors[0], 1.0f);
		deviceContextRef->Draw(object->models[object->lodIndex]->vertexBufferSize, 0);
	}

	dx->EndScene();
}

void LevelOfDetail::RenderCPNTLOD(LoDObject* object)
{
	dx->BeginScene(0.0f, 0.75f, 1.0f, 1.0f);
	SetTessellationCBPerFrame(viewMatrix, projectionMatrix);

	//Decide lod-level
	float3 camPos = camera.GetPosition();
	float length = camPos.Length();

	if (length > LOD_LEVELS[1])
		object->lodIndex = 2;
	else if (length < LOD_LEVELS[0])
		object->lodIndex = 0;
	else
		object->lodIndex = 1;

	//Set resources
	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;
	deviceContextRef->IASetVertexBuffers(0, 1, &object->models[object->lodIndex]->vertexBuffer, &vertexSize, &offset);
	if (object->texture)
		deviceContextRef->PSSetShaderResources(0, 1, &object->texture);

	//Render
	SetTessellationCBPerObject(worldMatrix, colors[0]);
	deviceContextRef->Draw(object->models[object->lodIndex]->vertexBufferSize, 0);

	dx->EndScene();
}

void LevelOfDetail::RenderPhongLOD(LoDObject* object)
{
	dx->BeginScene(0.0f, 0.75f, 1.0f, 1.0f);
	SetTessellationCBPerFrame(viewMatrix, projectionMatrix);

	//Decide lod-level
	float3 camPos = camera.GetPosition();
	float length = camPos.Length();

	if (length > LOD_LEVELS[1])
		object->lodIndex = 2;
	else if (length < LOD_LEVELS[0])
		object->lodIndex = 0;
	else
		object->lodIndex = 1;

	//Set resources
	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;
	deviceContextRef->IASetVertexBuffers(0, 1, &object->models[object->lodIndex]->vertexBuffer, &vertexSize, &offset);
	if (object->texture)
		deviceContextRef->PSSetShaderResources(0, 1, &object->texture);

	//Render
	SetTessellationCBPerObject(worldMatrix, colors[0]);
	deviceContextRef->Draw(object->models[object->lodIndex]->vertexBufferSize, 0);

	dx->EndScene();
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
		case 'B':
			objectIndex = 4;
			break;
		default:
			break;
	}
}

void LevelOfDetail::OnKeyUp(UINT8 key)
{
	camera.OnKeyUp(key);
	freelookCamera.OnKeyUp(key);
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
