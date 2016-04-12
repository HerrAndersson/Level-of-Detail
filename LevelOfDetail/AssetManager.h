#pragma once
#include "AssetUtil.h"
#include "stdafx.h"

using namespace std;
using namespace AssetUtility;

class AssetManager
{

private:

	vector<RenderObject*> renderObjects;

	void LoadModel(ID3D11Device* device, string file_path, Model* model);
	void LoadModelNoUV(ID3D11Device* device, string file_path, Model* model);
	ID3D11ShaderResourceView* LoadTexture(ID3D11Device* device, string file_path);

public:

	AssetManager();
	~AssetManager();

	void LoadObject(ID3D11Device* device, string modelPath, string texturePath);

	RenderObject* GetRenderObject(int id);
};

