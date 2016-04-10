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
		float3 position;
		float2 uv;
		float3 normal;
	};

	struct Model 
	{
		string name = "";
		int vertexBufferSize = 0;

		ComPtr<ID3D11Buffer> vertexBuffer;
	};

	struct RenderObject 
	{
		Model model;
		ComPtr<ID3D11ShaderResourceView> diffuseTexture = nullptr;
	};
}
