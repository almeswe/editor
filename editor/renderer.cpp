#include "renderer.h"

Renderer::Renderer(HWND window)
{
	this->CurrentFirstLine = 0;
	this->ScrollOffset = 0;

	this->FontSize = 35.0f;
	this->Window = window;
	this->CreateResources();
}
Renderer::~Renderer()
{
	free(this);
}
void Renderer::RenderText()
{
	GetClientRect(this->Window, &this->SurfaceRect);
	if (!this->Direct2DTarget)
	{
		this->CreateDirect2DTarget();
		this->CreateDirect2DBrush();
	}
	if (!this->Direct2DContext)
		this->CreateDirect2DContext();

	this->CreateDWriteTextLayout();
	this->RenderTextWithDirect2DContext();
}

#define ADDITION_HEIGHT 150
void Renderer::RenderText(Gap* gap)
{
	GetClientRect(this->Window, &this->SurfaceRect);
	if (!this->Direct2DTarget)
	{
		this->CreateDirect2DTarget();
		this->CreateDirect2DBrush();
	}
	if (!this->Direct2DContext)
		this->CreateDirect2DContext();

	float targetHeight = this->Direct2DTarget->GetSize().height;

	ComPtr<IDWriteTextLayout> PrTextLayout;
	DWRITE_TEXT_RANGE DWriteRange;
	DWriteRange.startPosition = 0;

	D2D1_POINT_2F paragraphPos = Point2F(0, this->ScrollOffset);

	this->Direct2DContext->BeginDraw();
	this->Direct2DContext->SetTransform(IdentityMatrix());
	this->Direct2DContext->Clear(ColorF(ColorF::DarkSlateGray));
	for (int pr = 0; pr < this->Paragraphs.size(); pr++)
	{
		if (paragraphPos.y + this->FontSize >= 0)
		{
			this->DWriteFactory->CreateTextLayout(
				this->Paragraphs[pr].Text.c_str(),
				this->Paragraphs[pr].Length,
				this->DWriteTextFormat,
				0,
				0,
				&PrTextLayout
			);
			DWriteRange.length = this->Paragraphs[pr].Length;
			PrTextLayout->SetFontSize(this->FontSize, DWriteRange);

			this->Direct2DContext->DrawTextLayout(
				paragraphPos,
				PrTextLayout.Get(),
				this->Direct2DBrush);

		}
		paragraphPos.y += this->FontSize;

		if (paragraphPos.y > targetHeight)
			break;
	}
	this->Direct2DContext->EndDraw();
}
void Renderer::RenderCursor(size_t pos)
{
	float CursorX, CursorY;
	DWRITE_HIT_TEST_METRICS CursorMetrics;
	DWriteTextLayout->HitTestTextPosition(
		pos,
		0,
		&CursorX,
		&CursorY,
		&CursorMetrics
	);

	/*DWriteTextLayout->HitTestPoint(


	);*/

	switch (this->DWriteTextFormat->GetFontStyle())
	{
		case DWRITE_FONT_STYLE_ITALIC:
			CursorX += this->FontSize / 20;
			break;
		case DWRITE_FONT_STYLE_OBLIQUE:
			CursorX += this->FontSize / 5;
			break;
	}

	this->Direct2DTarget->BeginDraw();
	this->Direct2DTarget->DrawLine(
		{ CursorX,CursorY + this->FontSize / 5 },
		{ CursorX,CursorY + this->FontSize },
		this->Direct2DBrush,
		this->FontSize / 20
	);
	this->Direct2DTarget->EndDraw();
}
void Renderer::SetText(wchar_t* text)
{
	this->Text = text;
}
void Renderer::SetParagraphs(vector<Paragraph> prs)
{
	this->Paragraphs = prs;
}

void Renderer::CreateDirect2DBrush()
{
	this->Direct2DTarget->CreateSolidColorBrush(
		ColorF(ColorF::WhiteSmoke),
		&(this->Direct2DBrush)
	);
}
void Renderer::CreateDirect2DTarget()
{
	D2D1_SIZE_U Direct2DSize = SizeU(
		this->SurfaceRect.right - this->SurfaceRect.left,
		this->SurfaceRect.bottom - this->SurfaceRect.top
	);

	this->Direct2DFactory->CreateHwndRenderTarget(
		RenderTargetProperties(),
		HwndRenderTargetProperties(
			this->Window,
			Direct2DSize
		),
		&this->Direct2DTarget
	);
}
void Renderer::CreateDirect2DFactory()
{
	D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&this->Direct2DFactory
	);
}
void Renderer::CreateDirect2DContext()
{
	this->Direct2DTarget->QueryInterface(
		__uuidof(ID2D1DeviceContext),
		reinterpret_cast<void**>(&this->Direct2DContext)
	);
}
void Renderer::RenderTextWithDirect2DContext()
{
	ID2D1DrawingStateBlock* Direct2DState;
	HRESULT hr = this->Direct2DFactory->CreateDrawingStateBlock(
		&Direct2DState
	);

	if (SUCCEEDED(hr))
		this->Direct2DContext->SaveDrawingState(Direct2DState);
	this->Direct2DContext->BeginDraw();
	this->Direct2DContext->SetTransform(IdentityMatrix());
	this->Direct2DContext->Clear(ColorF(ColorF::DarkSlateGray));
	this->Direct2DContext->DrawTextLayout(
		{ 0,0 },
		this->DWriteTextLayout.Get(),
		this->Direct2DBrush,
		D2D1_DRAW_TEXT_OPTIONS_NONE
	);
	this->Direct2DContext->EndDraw();
	if (SUCCEEDED(hr))
		this->Direct2DContext->RestoreDrawingState(Direct2DState);
}
void Renderer::RenderTextWithDirect2DTarget()
{
	ID2D1DrawingStateBlock* Direct2DState;
	HRESULT hr = this->Direct2DFactory->CreateDrawingStateBlock(
		&Direct2DState
	);

	D2D1_RECT_F Direct2DRectLayout = RectF(
		static_cast<FLOAT>(this->SurfaceRect.top),
		static_cast<FLOAT>(this->SurfaceRect.left),
		static_cast<FLOAT>(this->SurfaceRect.right - this->SurfaceRect.left),
		static_cast<FLOAT>(this->SurfaceRect.bottom - this->SurfaceRect.top)
	);
	if (SUCCEEDED(hr))
		this->Direct2DTarget->SaveDrawingState(Direct2DState);
	this->Direct2DTarget->BeginDraw();
	this->Direct2DTarget->SetTransform(IdentityMatrix());
	this->Direct2DTarget->Clear(ColorF(ColorF::DarkSlateGray));
	this->Direct2DTarget->DrawTextLayout(
		{ 0,0 },
		this->DWriteTextLayout.Get(),
		this->Direct2DBrush
	);
	this->Direct2DTarget->EndDraw();
	if (SUCCEEDED(hr))
		this->Direct2DTarget->RestoreDrawingState(Direct2DState);
}

void Renderer::CreateDWriteFactory()
{
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		(IUnknown**)(&this->DWriteFactory)
	);
}
void Renderer::CreateDWriteTextFormat()
{
	if (this->DWriteFactory)
	{
		this->DWriteFactory->CreateTextFormat(
			L"Consolas",
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			this->FontSize,
			L"en-us",
			&this->DWriteTextFormat
		);

		DWRITE_TRIMMING DWriteTrimming;
		DWriteTrimming.delimiter = 0;
		DWriteTrimming.delimiterCount = 0;
		DWriteTrimming.granularity = DWRITE_TRIMMING_GRANULARITY_NONE;

		this->DWriteTextFormat->SetTrimming(&DWriteTrimming, NULL);
		this->DWriteTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
		this->DWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		this->DWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}
}
void Renderer::CreateDWriteTextLayout()
{
	DWRITE_TEXT_RANGE DWriteRange;
	DWriteRange.startPosition = 0;
	DWriteRange.length = wcslen(this->Text);

	this->DWriteFactory->CreateTextLayout(
		this->Text,
		wcslen(this->Text),
		this->DWriteTextFormat,
		0,
		0,
		&this->DWriteTextLayout
	);
	this->DWriteTextLayout->SetFontSize(this->FontSize, DWriteRange);
}

void Renderer::CreateResources()
{
	this->CreateDWriteFactory();
	this->CreateDirect2DFactory();
	this->CreateDWriteTextFormat();
}
void Renderer::DiscardResources()
{
	//rewrite
	this->Direct2DBrush->Release();
	this->Direct2DTarget->Release();
}

void Renderer::OnResize(UINT width,UINT height)
{
	if (this->Direct2DTarget)
	{
		D2D1_SIZE_U size;
		size.width = width;
		size.height = height;
		this->Direct2DTarget->Resize(size);
	}
}
void Renderer::OnScroll(FLOAT delta)
{
	this->ScrollOffset += delta == 65416.0f ? -this->FontSize : this->FontSize;
}
void Renderer::OnCtrlScroll(FLOAT delta)
{
	//normalazing
	delta = delta == 65416.0f ? -5.0f : 5.0f;
	if (this->FontSize + delta < 100.0f && this->FontSize + delta > 10.0f)
		this->FontSize += delta;
}