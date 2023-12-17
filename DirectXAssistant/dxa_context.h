#pragma once

#ifndef DXA_CONTEXT_H
#define DXA_CONTEXT_H

#ifdef __cplusplus

#include "dxa_base.h"
#include <d3d11_4.h>

class DXA_EXPORT DXADevice // 使用设备像素而非DIP像素
{
public:
	DXADevice();
	~DXADevice();
	DXADevice(const DXADevice&) = delete;
	DXADevice& operator=(const DXADevice&) = delete;
	void Initialize();
	void Uninitialize();
	ID3D11Device* D3Device();
	ID3D11DeviceContext* D3DContext();
	IDXGIDevice3* DXGIDevice();
	IDXGIAdapter* DXGIAdapter();
	IDXGIFactory2* DXGIFactory();
	operator ID3D11Device* ();
	operator ID3D11DeviceContext* ();
	operator IDXGIDevice3* ();
	operator IDXGIAdapter* ();
	operator IDXGIFactory2* ();

private:
	ID3D11Device* p3Device;
	ID3D11DeviceContext* p3DContext;
	IDXGIDevice3* pDXGIDevice;
	IDXGIAdapter* pDXGIAdapter;
	IDXGIFactory2* pDXGIFactory;
	D3D_FEATURE_LEVEL currentLevel;

	void ZeroData();
};

class DXA_EXPORT DXAContext // 使用设备像素而非DIP像素
{
public:
	DXAContext();
	~DXAContext();
	DXAContext(const DXAContext&) = delete;
	DXAContext& operator=(const DXAContext&) = delete;
	void Initialize(DXADevice* Device, HWND hWnd);
	void Uninitialize();
	void BeginDraw();
	bool EndDraw();
	void Clear(D2D1_COLOR_F Color = D2D1::ColorF(D2D1::ColorF::WhiteSmoke));
	void FillBitmap(ID2D1Bitmap* pBitmap, D2D1_RECT_F dstRect, D2D1_RECT_F srcRect = { 0 });
	D2D1_RECT_F PutBitmap(ID2D1Bitmap* pBitmap, D2D1_RECT_F dstRect, D2D1_RECT_F srcRect = { 0 });
	void DrawProgress(D2D1_RECT_F Rect, float Percentage, D2D1_COLOR_F BackColor, D2D1_COLOR_F FrontColor); // 0.0f ~ 100.0f
	HWND hWnd();
	//IDXGISwapChain1* DXGISwapChain();
	//IDXGISurface* DXGISurface();
	ID2D1Factory1* D2DFactory();
	ID2D1Device* D2Device();
	ID2D1DeviceContext* D2DContext();
	operator HWND();
	//operator IDXGISwapChain1* ();
	//operator IDXGISurface* ();
	operator ID2D1Factory1* ();
	operator ID2D1Device* ();
	operator ID2D1DeviceContext* ();

private:
	DXADevice* pDevice;
	HWND handle_hWnd;
	IDXGISwapChain1* pDXGISwapChain;
	IDXGISurface* pDXGISurface;
	ID2D1Factory1* p2DFactory;
	ID2D1Device* p2Device;
	ID2D1DeviceContext* p2DContext;
	ID2D1Bitmap1* pDrawBitmap;
	D2D1_BITMAP_PROPERTIES1 bitmapProperties;

	void ZeroData();
};

#endif // __cplusplus

#endif // !DXA_CONTEXT_H
