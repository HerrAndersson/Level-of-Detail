#pragma once

#include "stdafx.h"

namespace Renderer
{
	class DirectXHandler
	{
	private:

		ID3D11RenderTargetView*	backBufferRTV;
		ID3D11DepthStencilView*	backBufferDSV;
		
		IDXGISwapChain* swapChain;
		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;

		D3D11_VIEWPORT viewport;
		
		ID3D11DepthStencilState* depthEnable;
		ID3D11DepthStencilState* depthDisable;
		
		ID3D11RasterizerState* rsBack;
		ID3D11RasterizerState* rsFront;
		ID3D11RasterizerState* rsNone;
		ID3D11RasterizerState* rsWireframe;
										 
		ID3D11BlendState* blendEnable;
		ID3D11BlendState* blendDisable;

	public:

		enum RasterState { BACK, FRONT, NONE, WIREFRAME };
		enum State { ENABLED, DISABLED };

		DirectXHandler(HWND hwnd);
		~DirectXHandler();

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();

		void SetRasterState(RasterState state);
		void SetBlendState(State state);
		void SetDepthState(State state);

		void BeginScene(float red, float green, float blue, float alpha);
		void EndScene();
	};
}
