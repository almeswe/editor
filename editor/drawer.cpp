#include "drawer.h"

using namespace D2D1;

DWTextDrawer::DWTextDrawer(HWND window)
{
	this->FontSize = 25.0f;

	this->Window = window;
	this->DWriteFactory = NULL;
	this->Direct2DBrush = NULL;
	this->Direct2DTarget = NULL;
	this->Direct2DFactory = NULL;
	this->DWriteTextFormat = NULL;

	this->CreateResources();
}
DWTextDrawer::~DWTextDrawer()
{
	//rewrite
	free(this);
}
void DWTextDrawer::DWDrawText(const wchar_t* text)
{
	GetClientRect(this->Window, &(this->SurfaceRect));

	D2D1_SIZE_U size = SizeU(
		this->SurfaceRect.right - this->SurfaceRect.left,
		this->SurfaceRect.bottom - this->SurfaceRect.top
	);

	if (!Direct2DTarget)
	{
		Direct2DFactory->CreateHwndRenderTarget(
			RenderTargetProperties(),
			HwndRenderTargetProperties(
				this->Window,
				size
			),
			&Direct2DTarget
		);

		Direct2DTarget->CreateSolidColorBrush(
			ColorF(ColorF::WhiteSmoke),
			&Direct2DBrush
		);
	}

	Direct2DTarget->BeginDraw();
	Direct2DTarget->SetTransform(IdentityMatrix());
	Direct2DTarget->Clear(ColorF(ColorF::DarkSlateGray));
	D2D1_RECT_F layoutRect = RectF(
		static_cast<FLOAT>(this->SurfaceRect.top),
		static_cast<FLOAT>(this->SurfaceRect.left),
		static_cast<FLOAT>(this->SurfaceRect.right - this->SurfaceRect.left),
		static_cast<FLOAT>(this->SurfaceRect.bottom - this->SurfaceRect.top)
	);
	if (SUCCEEDED(DWriteFactory))
	{
		DWriteFactory->CreateTextFormat(
			L"Consolas",
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_ITALIC,
			DWRITE_FONT_STRETCH_NORMAL,
			this->FontSize,
			L"en-us",
			&DWriteTextFormat
		);
		DWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		DWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}
	Direct2DTarget->DrawTextW(
		text,
		wcslen(text),
		DWriteTextFormat,
		layoutRect,
		Direct2DBrush
	);

	Direct2DTarget->EndDraw();
}
void DWTextDrawer::CreateResources()
{
	HRESULT hr = NULL;
	//Direct2D factory
	D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&Direct2DFactory
	);
	//Direct write factory
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		(IUnknown**)(&DWriteFactory)
	);
	//Direct write text format & few options
	if (SUCCEEDED(DWriteFactory))
	{
		DWriteFactory->CreateTextFormat(
			L"Consolas",
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			this->FontSize,
			L"en-us",
			&DWriteTextFormat
		);
		DWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		DWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}
}
void DWTextDrawer::DiscardResources()
{
	//rewrite
	Direct2DBrush->Release();
	Direct2DTarget->Release();
}

void DWTextDrawer::OnResize(UINT width,UINT height)
{
	if (this->Direct2DTarget)
	{
		D2D1_SIZE_U size;
		size.width = width;
		size.height = height;
		this->Direct2DTarget->Resize(size);
	}
}
void DWTextDrawer::OnScroll(FLOAT delta)
{
	if (this->FontSize + delta < 100.0f && this->FontSize + delta > 10.0f)
		this->FontSize += delta;
}