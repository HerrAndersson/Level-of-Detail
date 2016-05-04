#pragma once
#include "stdafx.h"

namespace Renderer
{
	struct VertexShaderData
	{
		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11InputLayout* inputLayout = nullptr;

		VertexShaderData() {}
		VertexShaderData(ID3D11VertexShader* vs, ID3D11InputLayout* il)
		{
			vertexShader = vs;
			inputLayout = il;
		}

		~VertexShaderData(){}
	};

	enum SamplerStates { WRAP, POINT };

	class ShaderHandler
	{

	public:

		static VertexShaderData* CreateVertexShader(ID3D11Device* device, const std::wstring& fileName, D3D11_INPUT_ELEMENT_DESC* inputDesc, int inputDescSize, UINT compileFlags);
		static ID3D11HullShader* CreateHullShader(ID3D11Device* device, const std::wstring& fileName, UINT compileFlags);
		static ID3D11GeometryShader* CreateGeometryShader(ID3D11Device* device, const std::wstring& fileName, UINT compileFlags);
		static ID3D11DomainShader* CreateDomainShader(ID3D11Device* device, const std::wstring& fileName, UINT compileFlags);
		static ID3D11PixelShader* CreatePixelShader(ID3D11Device* device, const std::wstring& fileName, UINT compileFlags);
		static ID3D11ComputeShader* CreateComputeShader(ID3D11Device* device, const std::wstring& fileName, UINT compileFlags);

		static ID3D11SamplerState* CreateSamplerState(ID3D11Device* device, SamplerStates state);
	};
}