#pragma once
#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>
#include "Math.h"

using namespace DirectX;
using namespace Math;

namespace AssetUtility 
{
	struct Vertex
	{
		XMFLOAT3 position = { 0,0,0 };
		XMFLOAT2 uv = { 0,0 };
		XMFLOAT3 normal = { 0,0,0 };
	};

	struct Model 
	{
		string name = "";
		int vertexBufferSize = 0;
		ID3D11Buffer* vertexBuffer;
	};

	struct RenderObject 
	{
		Model* model;
		ID3D11ShaderResourceView* diffuseTexture = nullptr;

		~RenderObject()
		{
			SAFE_RELEASE(diffuseTexture);
			SAFE_RELEASE(model->vertexBuffer);
			delete model;
		}
	};
}
