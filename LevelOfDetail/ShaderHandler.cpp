#include "ShaderHandler.h"

namespace Renderer
{
	ID3D11SamplerState* ShaderHandler::CreateSamplerState(ID3D11Device* device, SamplerStates state)
	{
		HRESULT result;
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		ID3D11SamplerState* samplerState;

		switch (state)
		{
		case Renderer::WRAP:
		{
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			samplerDesc.BorderColor[0] = 0;
			samplerDesc.BorderColor[1] = 0;
			samplerDesc.BorderColor[2] = 0;
			samplerDesc.BorderColor[3] = 0;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

			result = device->CreateSamplerState(&samplerDesc, &samplerState);
			if (FAILED(result))
				throw std::runtime_error("ShaderHandler::CreateSamplerState: WRAP initialization failed.");

			return samplerState;
			
			break;
		}

		case Renderer::POINT:
		{
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.BorderColor[0] = 0;
			samplerDesc.BorderColor[1] = 0;
			samplerDesc.BorderColor[2] = 0;
			samplerDesc.BorderColor[3] = 0;
			samplerDesc.MinLOD = 0.0f;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MaxAnisotropy = 0;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

			result = device->CreateSamplerState(&samplerDesc, &samplerState);
			if (FAILED(result))
				throw std::runtime_error("ShaderHandler::CreateSamplerState: POINT initialization failed.");

			return samplerState;

			break;
		}
		default:
			break;
		}

		return nullptr;
	}

	Renderer::VertexShaderData* ShaderHandler::CreateVertexShader(ID3D11Device* device, const std::wstring& fileName, D3D11_INPUT_ELEMENT_DESC* inputDesc, int inputDescSize, UINT compileFlags)
	{
		HRESULT result;
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* shaderBuffer = nullptr;
		ID3D11VertexShader* vertexShader;
		ID3D11InputLayout* inputLayout;

		result = D3DCompileFromFile(fileName.c_str(), NULL, NULL, "main", "vs_5_0", compileFlags, 0, &shaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				throw std::runtime_error("ShaderHandler::CreateVertexShader: " + std::string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			}
			else
			{
				std::string s(fileName.begin(), fileName.end());
				throw std::runtime_error("ShaderHandler::CreateVertexShader: Shader file not found. Filename: " + s);
			}
		}

		result = device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &vertexShader);
		if (FAILED(result))
		{
			std::string s(fileName.begin(), fileName.end());
			throw std::runtime_error("ShaderHandler::CreateVertexShader: Shader file not found. Filename: " + s);
		}

		result = device->CreateInputLayout(inputDesc, inputDescSize, shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), &inputLayout);
		if (FAILED(result))
		{
			throw std::runtime_error("ShaderHandler::CreateVertexShader: Error creating input layout.");
		}

		SAFE_RELEASE(errorMessage);
		SAFE_RELEASE(shaderBuffer);

		return new VertexShaderData(vertexShader, inputLayout);
	}
	ID3D11HullShader* ShaderHandler::CreateHullShader(ID3D11Device* device, const std::wstring& fileName, UINT compileFlags)
	{
		HRESULT result;
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* shaderBuffer = nullptr;
		ID3D11HullShader* hullShader;

		result = D3DCompileFromFile(fileName.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "hs_5_0", compileFlags, 0, &shaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				throw std::runtime_error("ShaderHandler::CreateHullShader: " + std::string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			}
			else
			{
				std::string s(fileName.begin(), fileName.end());
				throw std::runtime_error("ShaderHandler::CreateHullShader: Shader file not found. Filename: " + s);
			}
		}

		result = device->CreateHullShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &hullShader);
		if (FAILED(result))
		{
			std::string s(fileName.begin(), fileName.end());
			throw std::runtime_error("ShaderHandler::CreateHullShader: Shader file not found. Filename: " + s);
		}

		SAFE_RELEASE(errorMessage);
		SAFE_RELEASE(shaderBuffer);

		return hullShader;
	}
	ID3D11GeometryShader* ShaderHandler::CreateGeometryShader(ID3D11Device* device, const std::wstring& fileName, UINT compileFlags)
	{
		HRESULT result;
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* shaderBuffer = nullptr;
		ID3D11GeometryShader* geometryShader;

		result = D3DCompileFromFile(fileName.c_str(), NULL, NULL, "main", "gs_5_0", compileFlags, 0, &shaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				throw std::runtime_error("ShaderHandler::CreateGeometryShader: " + std::string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			}
			else
			{
				std::string s(fileName.begin(), fileName.end());
				throw std::runtime_error("ShaderHandler::CreateGeometryShader: Shader file not found. Filename: " + s);
			}
		}

		result = device->CreateGeometryShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &geometryShader);
		if (FAILED(result))
		{
			std::string s(fileName.begin(), fileName.end());
			throw std::runtime_error("ShaderHandler::CreateGeometryShader: Shader file not found.. Filename: " + s);
		}

		SAFE_RELEASE(errorMessage);
		SAFE_RELEASE(shaderBuffer);

		return geometryShader;
	}
	ID3D11DomainShader* ShaderHandler::CreateDomainShader(ID3D11Device* device, const std::wstring& fileName, UINT compileFlags)
	{
		HRESULT result;
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* shaderBuffer = nullptr;
		ID3D11DomainShader* domainShader;

		result = D3DCompileFromFile(fileName.c_str(), NULL, NULL, "main", "ds_5_0", compileFlags, 0, &shaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				throw std::runtime_error("ShaderHandler::CreateDomainShader: " + std::string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			}
			else
			{
				std::string s(fileName.begin(), fileName.end());
				throw std::runtime_error("ShaderHandler::CreateDomainShader: Shader file not found. Filename: " + s);
			}
		}

		result = device->CreateDomainShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &domainShader);
		if (FAILED(result))
		{
			std::string s(fileName.begin(), fileName.end());
			throw std::runtime_error("ShaderHandler::CreateDomainShader: Shader file not found. Filename: " + s);
		}

		SAFE_RELEASE(errorMessage);
		SAFE_RELEASE(shaderBuffer);

		return domainShader;
	}
	ID3D11PixelShader* ShaderHandler::CreatePixelShader(ID3D11Device* device, const std::wstring& fileName, UINT compileFlags)
	{
		HRESULT result;
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* shaderBuffer = nullptr;
		ID3D11PixelShader* pixelShader;

		result = D3DCompileFromFile(fileName.c_str(), NULL, NULL, "main", "ps_5_0", compileFlags, 0, &shaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				throw std::runtime_error("ShaderHandler::CreatePixelShader: " + std::string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			}
			else
			{
				std::string s(fileName.begin(), fileName.end());
				throw std::runtime_error("ShaderHandler::CreatePixelShader: Shader file not found. Filename: " + s);
			}
		}

		result = device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &pixelShader);
		if (FAILED(result))
		{
			std::string s(fileName.begin(), fileName.end());
			throw std::runtime_error("ShaderHandler::CreatePixelShader: Shader file not found. Filename: " + s);
		}

		SAFE_RELEASE(errorMessage);
		SAFE_RELEASE(shaderBuffer);

		return pixelShader;
	}
	ID3D11ComputeShader* ShaderHandler::CreateComputeShader(ID3D11Device* device, const std::wstring& fileName, UINT compileFlags)
	{
		HRESULT result;
		ID3DBlob* errorMessage = nullptr;
		ID3DBlob* shaderBuffer = nullptr;
		ID3D11ComputeShader* computeShader;

		result = D3DCompileFromFile(fileName.c_str(), NULL, NULL, "main", "cs_5_0", compileFlags, 0, &shaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				throw std::runtime_error("ShaderHandler::CreateComputeShader: " + std::string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			}
			else
			{
				std::string s(fileName.begin(), fileName.end());
				throw std::runtime_error("ShaderHandler::CreateComputeShader: Shader file not found. Filename: " + s);
			}
		}

		result = device->CreateComputeShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &computeShader);
		if (FAILED(result))
		{
			std::string s(fileName.begin(), fileName.end());
			throw std::runtime_error("ShaderHandler::CreateComputeShader: Shader file not found. Filename: " + s);
		}

		SAFE_RELEASE(errorMessage);
		SAFE_RELEASE(shaderBuffer);

		return computeShader;
	}
}
