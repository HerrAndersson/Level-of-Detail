#include "AssetManager.h"
#include "WICTextureLoader.h"
#include "Utility.h"

Model* AssetManager::LoadModel(ID3D11Device* device, string file_path)
{
	ifstream infile;
	infile.open(file_path.c_str(), ifstream::binary);

	if (!infile.is_open())
	{
		string outputstring = file_path + " not found.\n";
		throw runtime_error("AssetManager::LoadModel: " + outputstring);
	}

	vector<XMFLOAT3> vertices;
	vector<XMFLOAT3> normals;
	vector<XMFLOAT2> uvs;
	vector<Vertex> faces;

	if (infile.good())
	{
		while (!infile.eof())
		{
			string command;
			infile >> command;
			if (command == "v")
			{
				XMFLOAT3 tempVertex;
				infile >> tempVertex.x;
				infile >> tempVertex.y;
				infile >> tempVertex.z;
				tempVertex.z *= -1.0f;
				vertices.push_back(tempVertex);
			}
			else if (command == "vt")
			{
				XMFLOAT2 tempUv;
				infile >> tempUv.x;
				infile >> tempUv.y;
				tempUv.y = 1.0f - tempUv.y;
				uvs.push_back(tempUv);
			}
			else if (command == "vn")
			{
				XMFLOAT3 tempNormal;
				infile >> tempNormal.x;
				infile >> tempNormal.y;
				infile >> tempNormal.z;
				tempNormal.z *= -1.0f;
				normals.push_back(tempNormal);
			}
			else if (command == "f")
			{
				int vertexIndex[3] = { 0,0,0 };
				int uvIndex[3] = { 0,0,0 };
				int normalIndex[3] = { 0,0,0 };

				for (int i = 0; i < 3; i++)
				{
					infile >> vertexIndex[i];
					//Check if we can load UV coordinates
					if (infile.peek() == '/')
					{
						infile.get();
						infile >> uvIndex[i];
						//Check if we can load normals
						if (infile.peek() == '/')
						{
							infile.get();
							infile >> normalIndex[i];
						}
					}
				}
				for (int i = 2; i >= 0; i--)
				{
					Vertex tempVertex;
					tempVertex.position = vertices[vertexIndex[i] - 1];
					tempVertex.uv = uvs[uvIndex[i] - 1];
					tempVertex.normal = normals[normalIndex[i] - 1];
					faces.push_back(tempVertex);
				}
			}
			else //Unknown command, ignore it
			{
				string tempString;
				getline(infile, tempString);
			}
		}
	}

	Model* model = new Model();
	model->vertexBufferSize = (int)faces.size();
	infile.close();

	ID3D11Buffer* vertexBuffer;
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = (UINT)(sizeof(Vertex) * faces.size());

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = faces.data();
	HRESULT result = device->CreateBuffer(&bufferDesc, &data, &vertexBuffer);

	if (FAILED(result))
		throw runtime_error("AssetManager::LoadModel: Failed to create vertexBuffer");

	model->vertexBuffer = vertexBuffer;

	return model;
}

Model* AssetManager::LoadModelNoUV(ID3D11Device* device, string file_path)
{
	ifstream infile;
	infile.open(file_path.c_str(), ifstream::binary);

	if (!infile.is_open())
	{
		string outputstring = file_path + " not found.\n";
		throw runtime_error("AssetManager::LoadModelNoUV: " + outputstring);
	}

	vector<XMFLOAT3> vertices;
	vector<XMFLOAT3> normals;
	vector<Vertex> faces;

	if (infile.good())
	{
		while (!infile.eof())
		{
			string command;
			infile >> command;
			if (command == "v")
			{
				XMFLOAT3 tempVertex;
				infile >> tempVertex.x;
				infile >> tempVertex.y;
				infile >> tempVertex.z;
				tempVertex.z *= -1.0f;
				vertices.push_back(tempVertex);
			}
			else if (command == "vn")
			{
				XMFLOAT3 tempNormal;
				infile >> tempNormal.x;
				infile >> tempNormal.y;
				infile >> tempNormal.z;
				tempNormal.z *= -1.0f;
				normals.push_back(tempNormal);
			}
			else if (command == "f")
			{
				int vertexIndex[3] = { 0,0,0 };
				int normalIndex[3] = { 0,0,0 };

				for (int i = 0; i < 3; i++)
				{
					infile >> vertexIndex[i];
					//Check if we can load normals
					if (infile.peek() == '/')
					{
						infile.get();
						infile >> normalIndex[i];
					}
				}
				for (int i = 2; i >= 0; i--)
				{
					Vertex tempVertex;
					tempVertex.position = vertices[vertexIndex[i] - 1];
					tempVertex.uv = { 0,0 };
					tempVertex.normal = normals[normalIndex[i] - 1];
					faces.push_back(tempVertex);
				}
			}
			else //Unknown command, ignore it
			{
				string tempString;
				getline(infile, tempString);
			}
		}
	}

	Model* model = new Model();
	model->vertexBufferSize = (int)faces.size();
	infile.close();

	ID3D11Buffer* vertexBuffer;
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = (UINT)(sizeof(Vertex) * faces.size());

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = faces.data();
	HRESULT result = device->CreateBuffer(&bufferDesc, &data, &vertexBuffer);
	if (FAILED(result))
		throw runtime_error("AssetManager::LoadModelNoUV: Failed to create vertexBuffer. " + GetErrorMessageFromHRESULT(result));

	model->vertexBuffer = vertexBuffer;

	return model;
}

ID3D11ShaderResourceView* AssetManager::LoadTexture(ID3D11Device* device, string file_path)
{
	HRESULT hr;
	ID3D11ShaderResourceView* texture;
	wstring widestr = wstring(file_path.begin(), file_path.end());

	hr = DirectX::CreateWICTextureFromFile(device, widestr.c_str(), nullptr, &texture, 0);
	if (FAILED(hr))
		throw runtime_error("AssetManager::LoadTexture: Failed to Load texture. " + GetErrorMessageFromHRESULT(hr));

	return texture;
}
