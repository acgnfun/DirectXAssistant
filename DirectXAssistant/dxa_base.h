#pragma once

#ifndef DXA_BASE_H
#define DXA_BASE_H

#ifdef __cplusplus

#include <d2d1_3.h>
#include <dwrite_3.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define CreateCentralWindow(lpClassName, lpWindowName, dwStyle, nWidth, nHeight,\
	hWndParent, hMenu, hInstance, lpParam)\
	CreateWindowExW(0L, lpClassName, lpWindowName, dwStyle, \
	(GetSystemMetrics(SM_CXFULLSCREEN) - (nWidth)) / 2, \
	(GetSystemMetrics(SM_CYFULLSCREEN) - (nHeight)) / 2, \
	nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)
#define CreateCentralWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, nWidth, nHeight,\
	hWndParent, hMenu, hInstance, lpParam)\
	CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, \
	(GetSystemMetrics(SM_CXFULLSCREEN) - (nWidth)) / 2, \
	(GetSystemMetrics(SM_CYFULLSCREEN) - (nHeight)) / 2, \
	nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)

#ifdef DXA_BUILD
#define DXA_EXPORT __declspec(dllexport)
#else
#define DXA_EXPORT __declspec(dllimport)
#endif // DXA_BUILD

class DXA_EXPORT DXAFontCollection
{
public:
	using DWriteMemoryLoader = IDWriteInMemoryFontFileLoader;
	using DWriteFontCollection = IDWriteFontCollection1;

	friend DXA_EXPORT HRESULT DXACreateFontCollection(IDWriteFactory5* pWriteFactory, LPCWSTR szPath, DXAFontCollection* pFontCollection, LPWSTR FontFamilyBuffer, UINT* BufElemNum);
	friend DXA_EXPORT HRESULT DXACreateFontCollection(IDWriteFactory5* pWriteFactory, HMODULE hModule, LPCWSTR szResourceName, LPCWSTR szResourceType, DXAFontCollection* pFontCollection, LPWSTR FontFamilyBuffer, UINT* BufElemNum);

	DXAFontCollection();
	~DXAFontCollection();
	DXAFontCollection(const DXAFontCollection&) = delete;
	DXAFontCollection& operator=(const DXAFontCollection&) = delete;
	void Release();
	DWriteFontCollection* FontCollection();
	operator DWriteFontCollection* ();
private:
	DWriteMemoryLoader* pMemoryLoader;
	DWriteFontCollection* pFontCollection;
	IDWriteFactory5* pWriteFactory;
};

class DXA_EXPORT DXAWICFactory
{
public:
	using WICFactory = IWICImagingFactory;
	DXAWICFactory();
	~DXAWICFactory();
	DXAWICFactory(const DXAWICFactory&) = delete;
	DXAWICFactory& operator=(const DXAWICFactory&) = delete;
	bool Initialize();
	void Uninitialize();
	WICFactory* ImagingFactory();
	operator WICFactory* ();
private:
	WICFactory* pImagingFactory;
};

DXA_EXPORT HRESULT DXACreateBitmap(IWICImagingFactory* pImagingFactory, ID2D1RenderTarget* pRenderTarget, LPCWSTR szPath, ID2D1Bitmap** ppBitmap);
DXA_EXPORT HRESULT DXACreateBitmap(IWICImagingFactory* pImagingFactory, ID2D1RenderTarget* pRenderTarget, HMODULE hModule, LPCWSTR szResourceName, LPCWSTR szResourceType, ID2D1Bitmap** ppBitmap);
DXA_EXPORT HRESULT DXACreateFontCollection(IDWriteFactory5* pWriteFactory, LPCWSTR szPath, DXAFontCollection* pFontCollection, LPWSTR FontFamilyBuffer, UINT* BufElemNum);
DXA_EXPORT HRESULT DXACreateFontCollection(IDWriteFactory5* pWriteFactory, HMODULE hModule, LPCWSTR szResourceName, LPCWSTR szResourceType, DXAFontCollection* pFontCollection, LPWSTR FontFamilyBuffer, UINT* BufElemNum);

#endif // __cplusplus

#endif // !DXA_BASE_H
