#pragma once
#include "stdafx.h"

namespace Renderer
{
	struct VertexShaderData
	{
		ComPtr<ID3D11VertexShader> vertexShader;
		ComPtr<ID3D11InputLayout> inputLayout;

		VertexShaderData() {}
		VertexShaderData(ComPtr<ID3D11VertexShader>, ComPtr<ID3D11InputLayout>)
		{
			vertexShader = vertexShader;
			inputLayout = inputLayout;
		}

		~VertexShaderData(){}
	};

	enum SamplerStates { WRAP, POINT };

	class ShaderHandler
	{

	public:

		static VertexShaderData* CreateVertexShader(ComPtr<ID3D11Device>, const std::wstring& fileName, D3D11_INPUT_ELEMENT_DESC* inputDesc, int inputDescSize, UINT compileFlags);
		static ComPtr<ID3D11HullShader> CreateHullShader(ComPtr<ID3D11Device> device, const std::wstring& fileName, UINT compileFlags);
		static ComPtr<ID3D11GeometryShader> CreateGeometryShader(ComPtr<ID3D11Device> device, const std::wstring& fileName, UINT compileFlags);
		static ComPtr<ID3D11DomainShader> CreateDomainShader(ComPtr<ID3D11Device> device, const std::wstring& fileName, UINT compileFlags);
		static ComPtr<ID3D11PixelShader> CreatePixelShader(ComPtr<ID3D11Device> device, const std::wstring& fileName, UINT compileFlags);
		static ComPtr<ID3D11ComputeShader> CreateComputeShader(ComPtr<ID3D11Device> device, const std::wstring& fileName, UINT compileFlags);

		static ComPtr<ID3D11SamplerState> CreateSamplerState(ComPtr<ID3D11Device> device, SamplerStates state);
	};
}