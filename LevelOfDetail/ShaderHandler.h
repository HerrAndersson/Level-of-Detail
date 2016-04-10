#ifndef SHADERHANDLER_H
#define SHADERHANDLER_H
#include <d3d11.h>
#include <string>
#include "stdafx.h"

namespace Renderer
{
	class ShaderHandler
	{
	private:

		struct VertexShaderData
		{
			ID3D11VertexShader* vertexShader;
			ID3D11InputLayout* inputLayout;

			VertexShaderData(ID3D11VertexShader* vertexShader, ID3D11InputLayout* inputLayout)
			{
				vertexShader = vertexShader;
				inputLayout = inputLayout;
			}

			~VertexShaderData()
			{
				SAFE_RELEASE(vertexShader);
				SAFE_RELEASE(inputLayout);
			}
		};

		//Samplers
		ID3D11SamplerState*		samplerWRAP;
		ID3D11SamplerState*		samplerPOINT;
		ID3D11SamplerState*		samplerCLAMP;
		ID3D11SamplerState*		samplerCMP;

		VertexShaderData* CreateVertexShader(ID3D11Device* device, const std::wstring& fileName, D3D11_INPUT_ELEMENT_DESC* inputDesc, int inputDescSize);
		ID3D11HullShader* CreateHullShader(ID3D11Device* device, const std::wstring& fileName);
		ID3D11GeometryShader* CreateGeometryShader(ID3D11Device* device, const std::wstring& fileName);
		ID3D11DomainShader* CreateDomainShader(ID3D11Device* device, const std::wstring& fileName);
		ID3D11PixelShader* CreatePixelShader(ID3D11Device* device, const std::wstring& fileName);
		ID3D11ComputeShader* CreateComputeShader(ID3D11Device* device, const std::wstring& fileName);

	public:

		ShaderHandler(ID3D11Device* device);
		~ShaderHandler();
	};
}

#endif