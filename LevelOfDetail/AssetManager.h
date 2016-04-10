#pragma once
#include "AssetUtil.h"
#include "stdafx.h"

using namespace std;
using namespace AssetUtility;

class AssetManager
{

private:

	vector<RenderObject> renderObjects;

	void LoadModel(ComPtr<ID3D11Device> device, string file_path, Model& model);
	ComPtr<ID3D11ShaderResourceView> LoadTexture(ComPtr<ID3D11Device> device, string file_path);

public:

	AssetManager();
	~AssetManager();

	void LoadObject(ComPtr<ID3D11Device> device, string modelPath, string texturePath);

	RenderObject* GetRenderObject(int id);
};

