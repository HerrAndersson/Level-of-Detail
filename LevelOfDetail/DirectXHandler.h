#pragma once

#include "stdafx.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

namespace Renderer
{
	class DirectXHandler
	{
	private:

		ComPtr<ID3D11RenderTargetView>	 backBufferRTV;
		ComPtr<ID3D11DepthStencilView>	 backBufferDSV;
		
		ComPtr<IDXGISwapChain>		     swapChain;
		ComPtr<ID3D11Device>			 device;
		ComPtr<ID3D11DeviceContext>		 deviceContext;

		D3D11_VIEWPORT					 viewport;
		
		ComPtr<ID3D11DepthStencilState>	 depthEnable;
		ComPtr<ID3D11DepthStencilState>	 depthDisable;
		
		ComPtr<ID3D11RasterizerState>	 rsBack;
		ComPtr<ID3D11RasterizerState>	 rsFront;
		ComPtr<ID3D11RasterizerState>	 rsNone;
		ComPtr<ID3D11RasterizerState>	 rsWireframe;
										 
		ComPtr<ID3D11BlendState>		 blendEnable;
		ComPtr<ID3D11BlendState>		 blendDisable;

	public:

		enum CullingState { BACK, FRONT, NONE, WIREFRAME };
		enum BlendState { ENABLE, DISABLE };

		DirectXHandler(HWND hwnd);
		~DirectXHandler();

		ComPtr<ID3D11Device> GetDevice();
		ComPtr<ID3D11DeviceContext> GetDeviceContext();

		void SetCullingState(CullingState state);
		void SetBlendState(BlendState state);

		void BeginScene(float red, float green, float blue, float alpha);
		void EndScene();
	};
}
