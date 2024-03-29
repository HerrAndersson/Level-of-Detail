#include "DirectXHandler.h"
#include "Utility.h"
#include <wincodec.h> //Holds codecs like GUID_ContainerFormatPng
#include "ScreenGrab.h" //Used for saving an ID3D11Texture2D to an image

namespace Renderer
{
	DirectXHandler::DirectXHandler(HWND hwnd)
	{
		HRESULT result;

		//////////////////////////////////////////////////// Swap chain, back buffer RTV //////////////////////////////////////////////////// 
		//Describe the swap chain
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = SCREEN_WIDTH;
		swapChainDesc.BufferDesc.Height = SCREEN_HEIGHT;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = true;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	
		//Create swap chain
		D3D_FEATURE_LEVEL featLvl = D3D_FEATURE_LEVEL_11_0;
		result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featLvl, 1, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, NULL, &deviceContext);
		if (FAILED(result))
			throw std::runtime_error("DirectXHandler: Error creating swap chain. " + GetErrorMessageFromHRESULT(result));

		//Get back buffer pointer and create render target view with it
		ID3D11Texture2D* backBufferPointer;
		result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPointer);
		if (FAILED(result))
			throw std::runtime_error("DirectXHandler: Could not get swap chain buffer pointer. " + GetErrorMessageFromHRESULT(result));

		result = device->CreateRenderTargetView(backBufferPointer, NULL, &rtvBackBuffer);
		if (FAILED(result))
			throw std::runtime_error("DirectXHandler: Error creating render target view. " + GetErrorMessageFromHRESULT(result));

		SAFE_RELEASE(backBufferPointer);

		///////////////////////////////////////////////////////// Depth buffer, DSV /////////////////////////////////////////////////////////
		ID3D11Texture2D* depthStencilBuffer;
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
		depthBufferDesc.Width = (UINT)SCREEN_WIDTH;
		depthBufferDesc.Height = (UINT)SCREEN_HEIGHT;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		//Create the texture for the depth buffer
		result = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
		if (FAILED(result))
			throw std::runtime_error("DirectXHandler: Error creating Depth texture. " + GetErrorMessageFromHRESULT(result));

		//Stencil view description
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		//Create the depth stencil view
		result = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &dsvBackBuffer);
		if (FAILED(result))
			throw std::runtime_error("DirectXHandler: Error creating Depth stencil view. " + GetErrorMessageFromHRESULT(result));

		SAFE_RELEASE(depthStencilBuffer);

		///////////////////////////////////////////////////////// Rasterizer-states ///////////////////////////////////////////////////////// 
		//Rasterizer setup
		D3D11_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(rasterDesc));

		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = true;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		//Create rasterizer states
		rasterDesc.CullMode = D3D11_CULL_BACK;
		result = device->CreateRasterizerState(&rasterDesc, &rsBack);
		if (FAILED(result))
			throw std::runtime_error("DirectXHandler: Error creating rasterizer state BACK. " + GetErrorMessageFromHRESULT(result));

		rasterDesc.CullMode = D3D11_CULL_FRONT;
		result = device->CreateRasterizerState(&rasterDesc, &rsFront);
		if (FAILED(result))
			throw std::runtime_error("DirectXHandler: Error creating rasterizer state FRONT. " + GetErrorMessageFromHRESULT(result));

		rasterDesc.CullMode = D3D11_CULL_NONE;
		result = device->CreateRasterizerState(&rasterDesc, &rsNone);
		if (FAILED(result))
			throw std::runtime_error("DirectXHandler: Error creating rasterizer state NONE. " + GetErrorMessageFromHRESULT(result));

		rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
		result = device->CreateRasterizerState(&rasterDesc, &rsWireframe);
		if (FAILED(result))
			throw std::runtime_error("DirectXHandler: Error creating rasterizer state WIREFRAME. " + GetErrorMessageFromHRESULT(result));

		/////////////////////////////////////////////////////// Depth stencil states ///////////////////////////////////////////////////////
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = false;
		depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace = depthStencilDesc.FrontFace;

		//Create depth state with test enabled and write enabled
		result = device->CreateDepthStencilState(&depthStencilDesc, &dssTestWrite);
		if (FAILED(result))
			throw std::runtime_error("DirectXHandler: Error creating dssTestWrite. " + GetErrorMessageFromHRESULT(result));

		//Create depth state with test enabled and write disabled
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		result = device->CreateDepthStencilState(&depthStencilDesc, &dssTestNoWrite);
		if (FAILED(result))
			throw std::runtime_error("DirectXHandler: Error creating dssTestNoWrite. " + GetErrorMessageFromHRESULT(result));

		////////////////////////////////////////////////////////// Blend-states //////////////////////////////////////////////////////////
		D3D11_BLEND_DESC omDesc;
		ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
		omDesc.RenderTarget[0].BlendEnable = true;
		omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		result = device->CreateBlendState(&omDesc, &bsAdditiveAlphaEnable);
		if (FAILED(result))
			throw std::runtime_error("DirectXHandler: Error creating blend ENABLE. " + GetErrorMessageFromHRESULT(result));

		//Create blend disable
		omDesc.RenderTarget[0].BlendEnable = false;
		result = device->CreateBlendState(&omDesc, &bsDisable);
		if (FAILED(result))
			throw std::runtime_error("DirectXHandler: Error creating blend DISABLE. " + GetErrorMessageFromHRESULT(result));

		//////////////////////////////////////////////////////////// Other ////////////////////////////////////////////////////////////
		//Set up viewport
		viewport = CD3D11_VIEWPORT(0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, 1.0f);

		deviceContext->RSSetState(rsBack);
		deviceContext->OMSetDepthStencilState(dssTestWrite, 0);
		deviceContext->RSSetViewports(1, &viewport);
		deviceContext->OMSetRenderTargets(1, &rtvBackBuffer, dsvBackBuffer);
	}

	DirectXHandler::~DirectXHandler()
	{
		swapChain->SetFullscreenState(false, NULL);

		SAFE_RELEASE(rtvBackBuffer);
		SAFE_RELEASE(dsvBackBuffer);
		SAFE_RELEASE(swapChain);
		SAFE_RELEASE(device);
		SAFE_RELEASE(deviceContext);
		SAFE_RELEASE(dssTestWrite);
		SAFE_RELEASE(dssTestNoWrite);
		SAFE_RELEASE(rsBack);
		SAFE_RELEASE(rsFront);
		SAFE_RELEASE(rsNone);
		SAFE_RELEASE(rsWireframe);
		SAFE_RELEASE(bsAdditiveAlphaEnable);
		SAFE_RELEASE(bsDisable);
	}

	ID3D11Device* DirectXHandler::GetDevice()
	{
		return device;
	}

	ID3D11DeviceContext* DirectXHandler::GetDeviceContext()
	{
		return deviceContext;
	}

	void DirectXHandler::SaveBackBufferToFile(wstring filename)
	{
		ID3D11Texture2D* backBuffer = nullptr;
		HRESULT hr = swapChain->GetBuffer(0, __uuidof(*backBuffer), (LPVOID*)&backBuffer);

		if (FAILED(hr))
			throw std::runtime_error("DirectXHandler::SaveBackBufferToFile: Failed to retrieve backbuffer. " + GetErrorMessageFromHRESULT(hr));

		hr = SaveWICTextureToFile(deviceContext, backBuffer, GUID_ContainerFormatPng, filename.c_str());

		if (FAILED(hr))
			throw std::runtime_error("DirectXHandler::SaveBackBufferToFile: Failed to save image. " + GetErrorMessageFromHRESULT(hr));

		SAFE_RELEASE(backBuffer);
	}

	void DirectXHandler::SetRasterState(RasterState state)
	{
		switch (state)
		{
		case Renderer::DirectXHandler::RasterState::CULL_BACK:
		{
			deviceContext->RSSetState(rsBack);
			break;
		}
		case Renderer::DirectXHandler::RasterState::CULL_FRONT:
		{
			deviceContext->RSSetState(rsFront);
			break;
		}
		case Renderer::DirectXHandler::RasterState::CULL_NONE:
		{
			deviceContext->RSSetState(rsNone);
			break;
		}
		case Renderer::DirectXHandler::RasterState::WIREFRAME:
		{
			deviceContext->RSSetState(rsWireframe);
			break;
		}
		default:
			break;
		}
	}

	void DirectXHandler::SetBlendState(BlendState state)
	{
		FLOAT factor[] = { 0,0,0,0 };
		//Passing NULL as factor gives a default value of (1,1,1,1)
		switch (state)
		{
		case Renderer::DirectXHandler::BlendState::ADDITIVE_ALPHA:
		{
			deviceContext->OMSetBlendState(bsAdditiveAlphaEnable, factor, 0xffffffff);
			break;
		}
		case Renderer::DirectXHandler::BlendState::DISABLED:
		{
			deviceContext->OMSetBlendState(bsDisable, factor, 0xffffffff);
			break;
		}
		default:
			break;
		}
	}

	void DirectXHandler::SetDepthState(DepthState state)
	{
		switch (state)
		{
		case Renderer::DirectXHandler::DepthState::TEST_WRITE:
		{
			deviceContext->OMSetDepthStencilState(dssTestWrite, 1);
			break;
		}
		case Renderer::DirectXHandler::DepthState::TEST_NO_WRITE:
		{
			deviceContext->OMSetDepthStencilState(dssTestNoWrite, 1);
			break;
		}
		default:
			break;
		}
	}

	void DirectXHandler::BeginScene(float red, float green, float blue, float alpha)
	{
		float color[] = { red, green, blue, alpha };
		deviceContext->ClearRenderTargetView(rtvBackBuffer, color);
		deviceContext->ClearDepthStencilView(dsvBackBuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void DirectXHandler::EndScene()
	{
		//V-sync
		//swapChain->Present(1, 0);

		swapChain->Present(0, 0);
	}
}