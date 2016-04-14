#pragma once
#include "AssetUtil.h"
#include "stdafx.h"

using namespace std;
using namespace AssetUtility;

class AssetManager
{
public:
	static Model* LoadModel(ID3D11Device* device, string file_path);
	static Model* LoadModelNoUV(ID3D11Device* device, string file_path);
	static ID3D11ShaderResourceView* LoadTexture(ID3D11Device* device, string file_path);

};

