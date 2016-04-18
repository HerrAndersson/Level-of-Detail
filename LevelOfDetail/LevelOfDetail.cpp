#include "stdafx.h"
#include "LevelOfDetail.h"

LevelOfDetail::LevelOfDetail(UINT width, UINT height, std::wstring name) :
	DXSample(width, height, name),
	activeTechnique(LoDTechnique::STATIC),
	rotation(0,0,0)
{
	for (int i = 0; i < 100; i++)
	{
		colors[i] = float3((RandomPercent() + 1) / 2, (RandomPercent() + 1) / 2, (RandomPercent() + 1) / 2);
	}

	freelookCameraActive = false;
}

void LevelOfDetail::OnInit()
{
	dx = new DirectXHandler(Win32Application::GetHwnd());
	deviceRef = dx->GetDevice();
	deviceContextRef = dx->GetDeviceContext();

	camera.Init({ 0.0f, 0.0f, -10.0f }, Camera::CameraMode::SCRIPTED);
	camera.SetMoveSpeed(0.1f);
	camera.SetTurnSpeed(XM_PI);
	freelookCamera.Init({ 0.0f, 0.0f, -10.0f }, Camera::CameraMode::MOUSE);
	freelookCamera.SetMoveSpeed(10.0f);
	freelookCamera.SetTurnSpeed(XM_PI);

	projectionMatrix = camera.GetProjectionMatrix(XM_PIDIV2, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	viewMatrix = camera.GetViewMatrix();

	LoadPipelineObjects();
	LoadAssets();

	//dx->SetRasterState(Renderer::DirectXHandler::RasterState::WIREFRAME);
	dx->SetBlendState(Renderer::DirectXHandler::BlendState::ADDITIVE_ALPHA);
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
		RenderNoLOD();
		break;
	}
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
	switch (activeTechnique)
	{
	case LoDTechnique::NO_LOD:
		RenderNoLOD();
		break;
	case LoDTechnique::STATIC:
		RenderStaticLOD();
		break;
	case LoDTechnique::UNPOPPING:
		RenderUnpoppingLOD();
		break;
	case LoDTechnique::CPNT:
		RenderCPNTLOD();
		break;
	case LoDTechnique::PHONG:
		RenderPhongLOD();
		break;
	default:
		RenderNoLOD();
		break;
	}

	float increment = XM_2PI / 360.0f;
	if (rotation.x < XM_PI && rotation.y >= XM_PI)
	{
		rotation.x += increment;
		rotation.y = 0;
	}
	else if (rotation.y < XM_PI && rotation.z >= XM_PI)
	{
		rotation.y += increment;
		rotation.z = 0;
	}
	else if (rotation.z < XM_PI)
	{
		rotation.z += increment;
	}


	worldMatrix = XMMatrixScaling(5.0f, 5.0f, 5.0f) * XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
}

void LevelOfDetail::RenderNoLOD()
{
	dx->BeginScene(0.0f, 0.75f, 1.0f, 1.0f);
	SetCBPerFrame(viewMatrix, projectionMatrix);

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
		worldMatrix *= XMMatrixTranslation(i * (colors[i].x * colors[i].y) , 0, i * colors[i].z);
		SetCBPerObject(worldMatrix, colors[i], 1.0f);
		deviceContextRef->Draw(object->models[0]->vertexBufferSize, 0);
	}

	dx->EndScene();
}

void LevelOfDetail::RenderStaticLOD()
{
	dx->BeginScene(0.0f, 0.75f, 1.0f, 1.0f);
	SetCBPerFrame(viewMatrix, projectionMatrix);

	LoDObject* object = lodObjects[0];

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

void LevelOfDetail::RenderUnpoppingLOD()
{
	dx->BeginScene(0.0f, 0.75f, 1.0f, 1.0f);
	SetCBPerFrame(viewMatrix, projectionMatrix);

	LoDObject* object = lodObjects[0];

	if (object->unpopBlendTimerActive)
	{
		if (object->unpopBlendTime > object->unpopBlendLimit)
		{
			object->unpopBlendTimerActive = false;
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
		//Decide lod-level of objects and update blend timer etc
		//Only works for objects at (0,0,0). If they should be spread out they need to hold their own position, and the length would be of the vector from camPos to objPos

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

void LevelOfDetail::RenderCPNTLOD()
{
	dx->BeginScene(0.0f, 0.75f, 1.0f, 1.0f);
	SetCBPerFrame(viewMatrix, projectionMatrix);

	LoDObject* object = lodObjects[0];

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

void LevelOfDetail::RenderPhongLOD()
{
	dx->BeginScene(0.0f, 0.75f, 1.0f, 1.0f);
	SetCBPerFrame(viewMatrix, projectionMatrix);



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
	deviceContextRef->PSSetShader(defaultPS, nullptr, 0);
	deviceContextRef->PSSetSamplers(0, 1, &samplerWrap);
	deviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void LevelOfDetail::SetStaticLOD()
{
	activeTechnique = LoDTechnique::STATIC;

	deviceContextRef->IASetInputLayout(defaultVS->inputLayout);
	deviceContextRef->VSSetShader(defaultVS->vertexShader, nullptr, 0);
	deviceContextRef->PSSetShader(defaultPS, nullptr, 0);
	deviceContextRef->PSSetSamplers(0, 1, &samplerWrap);
	deviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void LevelOfDetail::SetUnpoppingLOD()
{
	activeTechnique = LoDTechnique::UNPOPPING;

	deviceContextRef->IASetInputLayout(defaultVS->inputLayout);
	deviceContextRef->VSSetShader(defaultVS->vertexShader, nullptr, 0);
	deviceContextRef->PSSetShader(defaultPS, nullptr, 0);
	deviceContextRef->PSSetSamplers(0, 1, &samplerWrap);
	deviceContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void LevelOfDetail::SetCPNTLOD()
{
	activeTechnique = LoDTechnique::CPNT;
}

void LevelOfDetail::SetPhongLOD()
{
	activeTechnique = LoDTechnique::PHONG;
}

// Random percent value, from -1 to 1.
float LevelOfDetail::RandomPercent()
{
	float ret = static_cast<float>((rand() % 10000) - 5000);
	return ret / 5000.0f;
}
