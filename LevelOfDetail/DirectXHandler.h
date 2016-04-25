#pragma once

#include "stdafx.h"

namespace Renderer
{
	class DirectXHandler
	{
	private:

		ID3D11RenderTargetView*	rtvBackBuffer;
		ID3D11DepthStencilView*	dsvBackBuffer;
		
		IDXGISwapChain* swapChain;
		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;

		CD3D11_VIEWPORT viewport;
		
		ID3D11RasterizerState* rsBack;
		ID3D11RasterizerState* rsFront;
		ID3D11RasterizerState* rsNone;
		ID3D11RasterizerState* rsWireframe;

		ID3D11DepthStencilState* dssTestWrite;
		ID3D11DepthStencilState* dssTestNoWrite;
										 
		ID3D11BlendState* bsAdditiveAlphaEnable;
		ID3D11BlendState* bsDisable;

	public:

		enum RasterState { CULL_BACK, CULL_FRONT, CULL_NONE, WIREFRAME };
		enum BlendState { ADDITIVE_ALPHA, DISABLED };
		enum DepthState { TEST_WRITE, TEST_NO_WRITE };

		DirectXHandler(HWND hwnd);
		~DirectXHandler();

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetDeviceContext();
		void SaveBackBufferToFile(wstring filename);

		void SetRasterState(RasterState state);
		void SetBlendState(BlendState state);
		void SetDepthState(DepthState state);

		ID3D11DepthStencilState* GetDSSTestWrite();
		ID3D11DepthStencilState* GetDSSTestNoWrite();

		void BeginScene(float red, float green, float blue, float alpha);
		void EndScene();
	};
}
