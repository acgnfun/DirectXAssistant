#include "dxa_base.h"
#include <wincodec.h>
#include <wincodecsdk.h>

DXAFontCollection::DXAFontCollection()
{
	pMemoryLoader = nullptr;
	pFontCollection = nullptr;
	pWriteFactory = nullptr;
}

DXAFontCollection::~DXAFontCollection()
{
	Release();
}

void DXAFontCollection::Release()
{
	if (pFontCollection) pFontCollection->Release();
	if (pMemoryLoader)
	{
		pWriteFactory->UnregisterFontFileLoader(pMemoryLoader);
		pMemoryLoader->Release();
	}
	pFontCollection = nullptr;
	pMemoryLoader = nullptr;
	pWriteFactory = nullptr;
}

DXAFontCollection::DWriteFontCollection* DXAFontCollection::FontCollection()
{
	return pFontCollection;
}

DXAFontCollection::operator DWriteFontCollection* ()
{
	return pFontCollection;
}

DXAWICFactory::DXAWICFactory()
{
	pImagingFactory = nullptr;
}

DXAWICFactory::~DXAWICFactory()
{
	Uninitialize();
}

bool DXAWICFactory::Initialize()
{
	if (pImagingFactory) return true;
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pImagingFactory)
	);
	return SUCCEEDED(hr);
}

void DXAWICFactory::Uninitialize()
{
	if (pImagingFactory) pImagingFactory->Release();
	pImagingFactory = nullptr;
}

DXAWICFactory::WICFactory* DXAWICFactory::ImagingFactory()
{
	return pImagingFactory;
}

DXAWICFactory::operator WICFactory* ()
{
	return pImagingFactory;
}

HRESULT DXACreateBitmap(IWICImagingFactory* pImagingFactory, ID2D1RenderTarget* pRenderTarget, LPCWSTR szPath, ID2D1Bitmap** ppBitmap)
{
	if (!pImagingFactory) return E_FAIL;
	IWICBitmapDecoder* pDecoder = nullptr;
	IWICBitmapFrameDecode* pSource = nullptr;
	IWICFormatConverter* pConverter = nullptr;
	HRESULT hr = pImagingFactory->CreateDecoderFromFilename(
		szPath,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);
	if (SUCCEEDED(hr))
	{
		hr = pDecoder->GetFrame(0, &pSource);
	}
	if (SUCCEEDED(hr))
	{
		hr = pImagingFactory->CreateFormatConverter(&pConverter);
	}
	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(
			pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
		);
	}
	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
		);
	}
	if (pDecoder) pDecoder->Release();
	if (pSource) pSource->Release();
	if (pConverter) pConverter->Release();
	return hr;
}

HRESULT DXACreateBitmap(IWICImagingFactory* pImagingFactory, ID2D1RenderTarget* pRenderTarget, HMODULE hModule, LPCWSTR szResourceName, LPCWSTR szResourceType, ID2D1Bitmap** ppBitmap)
{
	if (!pImagingFactory) return E_FAIL;
	IWICBitmapDecoder* pDecoder = nullptr;
	IWICBitmapFrameDecode* pSource = nullptr;
	IWICFormatConverter* pConverter = nullptr;
	IWICStream* pStream = nullptr;
	HRSRC hRscBitmap = nullptr;
	HGLOBAL hGlobalBitmap = nullptr;
	void* pImage = nullptr;
	DWORD nImageSize = 0;
	hRscBitmap = FindResourceW(hModule, szResourceName, szResourceType);
	HRESULT hr = hRscBitmap ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		// Load the resource.
		hGlobalBitmap = LoadResource(hModule, hRscBitmap);
		hr = hGlobalBitmap ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr))
	{
		// Lock it to get a system memory pointer.
		pImage = LockResource(hGlobalBitmap);
		hr = pImage ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr))
	{
		// Calculate the size.
		nImageSize = SizeofResource(hModule, hRscBitmap);
		hr = nImageSize ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr))
	{
		// Create a WIC stream to map onto the memory.
		hr = pImagingFactory->CreateStream(&pStream);
	}
	if (SUCCEEDED(hr))
	{
		// Initialize the stream with the memory pointer and size.
		hr = pStream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(pImage),
			nImageSize
		);
	}
	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream.
		hr = pImagingFactory->CreateDecoderFromStream(
			pStream,
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
		);
	}
	if (SUCCEEDED(hr))
	{
		hr = pDecoder->GetFrame(0, &pSource);
	}
	if (SUCCEEDED(hr))
	{
		hr = pImagingFactory->CreateFormatConverter(&pConverter);
	}
	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(
			pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
		);
	}
	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
		);
	}
	if (pDecoder) pDecoder->Release();
	if (pSource) pSource->Release();
	if (pConverter) pConverter->Release();
	if (pStream) pStream->Release();
	return hr;
}

HRESULT DXACreateFontCollection(IDWriteFactory5* pWriteFactory, LPCWSTR szPath, DXAFontCollection* pFontCollection, LPWSTR FontFamilyBuffer, UINT* BufElemNum)
{
	IDWriteFontSetBuilder1* pFSBuilder = nullptr;
	IDWriteFontSet* pFontSet = nullptr;
	IDWriteFontFile* pFontFile = nullptr;
	IDWriteFontCollection1* pFCollection = nullptr;
	IDWriteFontFamily* pFFamily = nullptr;
	IDWriteLocalizedStrings* pFFName = nullptr;
	HRESULT hr = pWriteFactory->CreateFontSetBuilder(&pFSBuilder);
	if (SUCCEEDED(hr)) hr = pWriteFactory->CreateFontFileReference(szPath, /* lastWriteTime*/ nullptr, &pFontFile);
	if (SUCCEEDED(hr)) hr = pFSBuilder->AddFontFile(pFontFile);
	if (SUCCEEDED(hr)) hr = pFSBuilder->CreateFontSet(&pFontSet);
	if (SUCCEEDED(hr)) hr = pWriteFactory->CreateFontCollectionFromFontSet(pFontSet, &pFCollection);
	if (SUCCEEDED(hr)) hr = pFCollection->GetFontFamily(0, &pFFamily);
	if (SUCCEEDED(hr)) hr = pFFamily->GetFamilyNames(&pFFName);
	UINT32 len = 0;
	if (SUCCEEDED(hr)) hr = pFFName->GetStringLength(0, &len);
	if (*BufElemNum <= len)
	{
		*BufElemNum = len + 1;
		hr = E_FAIL;
	}
	if (SUCCEEDED(hr)) hr = pFFName->GetString(0, FontFamilyBuffer, *BufElemNum);
	if (pFSBuilder) pFSBuilder->Release();
	if (pFontSet) pFontSet->Release();
	if (pFontFile) pFontFile->Release();
	if (pFFamily) pFFamily->Release();
	if (pFFName) pFFName->Release();
	if (!SUCCEEDED(hr) && pFCollection) pFCollection->Release();
	if (SUCCEEDED(hr))
	{
		pFontCollection->pFontCollection = pFCollection;
		pFontCollection->pWriteFactory = pWriteFactory;
	}
	return hr;
}

HRESULT DXACreateFontCollection(IDWriteFactory5* pWriteFactory, HMODULE hModule, LPCWSTR szResourceName, LPCWSTR szResourceType, DXAFontCollection* pFontCollection, LPWSTR FontFamilyBuffer, UINT* BufElemNum)
{
	IDWriteFontSetBuilder1* pFSBuilder = nullptr;
	IDWriteFontSet* pFontSet = nullptr;
	IDWriteFontFile* pFontFile = nullptr;
	IDWriteFontCollection1* pFCollection = nullptr;
	IDWriteFontFamily* pFFamily = nullptr;
	IDWriteLocalizedStrings* pFFName = nullptr;
	HRSRC hRscFont = nullptr;
	HGLOBAL hGlobalFont = nullptr;
	void* pFontData = nullptr;
	DWORD nFDSize = 0;
	hRscFont = FindResourceW(hModule, szResourceName, szResourceType);
	HRESULT hr = hRscFont ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		// Load the resource.
		hGlobalFont = LoadResource(hModule, hRscFont);
		hr = hGlobalFont ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr))
	{
		// Lock it to get a system memory pointer.
		pFontData = LockResource(hGlobalFont);
		hr = pFontData ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr))
	{
		// Calculate the size.
		nFDSize = SizeofResource(hModule, hRscFont);
		hr = nFDSize ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr)) hr = pWriteFactory->CreateFontSetBuilder(&pFSBuilder);
	if (SUCCEEDED(hr)) hr = pWriteFactory->CreateInMemoryFontFileLoader(&pFontCollection->pMemoryLoader);
	if (SUCCEEDED(hr)) hr = pWriteFactory->RegisterFontFileLoader(pFontCollection->pMemoryLoader);
	if (SUCCEEDED(hr)) hr = pFontCollection->pMemoryLoader->CreateInMemoryFontFileReference(pWriteFactory, pFontData, nFDSize, nullptr, &pFontFile);
	if (SUCCEEDED(hr)) hr = pFSBuilder->AddFontFile(pFontFile);
	if (SUCCEEDED(hr)) hr = pFSBuilder->CreateFontSet(&pFontSet);
	if (SUCCEEDED(hr)) hr = pWriteFactory->CreateFontCollectionFromFontSet(pFontSet, &pFCollection);
	if (SUCCEEDED(hr)) hr = pFCollection->GetFontFamily(0, &pFFamily);
	if (SUCCEEDED(hr)) hr = pFFamily->GetFamilyNames(&pFFName);
	UINT32 len = 0;
	if (SUCCEEDED(hr)) hr = pFFName->GetStringLength(0, &len);
	if (*BufElemNum <= len)
	{
		*BufElemNum = len + 1;
		hr = E_FAIL;
	}
	if (SUCCEEDED(hr)) hr = pFFName->GetString(0, FontFamilyBuffer, *BufElemNum);
	if (pFSBuilder) pFSBuilder->Release();
	if (pFontSet) pFontSet->Release();
	if (pFontFile) pFontFile->Release();
	if (pFFamily) pFFamily->Release();
	if (pFFName) pFFName->Release();
	if (!SUCCEEDED(hr) && pFCollection) pFCollection->Release();
	if (SUCCEEDED(hr))
	{
		pFontCollection->pFontCollection = pFCollection;
		pFontCollection->pWriteFactory = pWriteFactory;
	}
	return hr;
}
