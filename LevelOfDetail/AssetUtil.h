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

	struct LoDObject 
	{
		Model* models[3];
		ID3D11ShaderResourceView* texture = nullptr;

		float unpopBlendTime = 0;
		float unpopBlendLimit = 0;
		bool unpopBlendTimerActive = false;

		int lodIndex = 0;
		int lodIndexPrevious = 0;

		LoDObject()
		{

		}

		~LoDObject()
		{
			SAFE_RELEASE(texture);

			for (auto m : models)
			{
				SAFE_RELEASE(m->vertexBuffer);
				delete m;
			}
		}
	};
}
