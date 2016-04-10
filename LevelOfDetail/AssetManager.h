#pragma once
#include "AssetUtil.h"

using namespace std;
using namespace AssetUtility;

class AssetManager
{

private:

	ComPtr<ID3D11Device> deviceRef;

	vector<Model*> models;
	vector<ID3D11ShaderResourceView*> textures;
	vector<RenderObject*> renderObjects;

	void LoadModel(string file_path, Model& model);
	ComPtr<ID3D11ShaderResourceView> LoadTexture(string file_path);

public:

	AssetManager(ComPtr<ID3D11Device> deviceRef);
	~AssetManager();

	RenderObject* GetRenderObject(int id);
};

