#include "renderer.h"

Renderer::Renderer(HWND window)
{
	this->ScrollOffset = 0;

	this->FontSize = 35.0f;
	this->Window = window;
	this->CreateResources();
}
Renderer::~Renderer()
{
	free(this);
}

void Renderer::OnResize(UINT width, UINT height)
{
	if (this->Direct2DTarget)
		this->Direct2DTarget->Resize({ width,height });
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
	{
		this->FontSize += delta;
		this->ScrollOffset -= delta;
	}
}

void Renderer::RenderText(Gap* gap)
{
	GetClientRect(this->Window, &this->SurfaceRect);
	if (!this->Direct2DTarget)
	{
		this->CreateDirect2DTarget();
		this->Direct2DBrush = this->CreateDirect2DBrush();
	}
	if (!this->Direct2DContext)
		this->CreateDirect2DContext();

	this->SetParagraphs(gap->GetParagraphs());
	this->RenderTextWithDirect2DContext();
}
void Renderer::RenderCursor(size_t pos)
{
	size_t textPos = 0;
	float cursorX, cursorY;
	float offsetY = this->ScrollOffset;
	ComPtr<IDWriteTextLayout> prLayout;

	for (Paragraph pr : this->Paragraphs)
	{
		if (textPos + pr.Length >= pos)
		{
			pos -= textPos;
			prLayout = pr.Layout;
			break;
		}
		else
			textPos += pr.Length;
		offsetY += this->FontSize;
	}

	DWRITE_HIT_TEST_METRICS cursorMetrics;
	prLayout->HitTestTextPosition(
		pos,
		0,
		&cursorX,
		&cursorY,
		&cursorMetrics
	);

	switch (this->DWriteTextFormat->GetFontStyle())
	{
		case DWRITE_FONT_STYLE_ITALIC:
			cursorX += this->FontSize / 20;
			break;
		case DWRITE_FONT_STYLE_OBLIQUE:
			cursorX += this->FontSize / 5;
			break;
	}

	this->Direct2DTarget->BeginDraw();
	this->Direct2DTarget->DrawLine(
		{ cursorX,offsetY + cursorY + this->FontSize / 5 },
		{ cursorX,offsetY + cursorY + this->FontSize },
		this->Direct2DBrush.Get(),
		this->FontSize / 20
	);
	this->Direct2DTarget->EndDraw();
}
void Renderer::SetParagraphs(vector<Paragraph> prs)
{
	for (size_t pr = 0;pr < prs.size(); pr++)
		prs[pr].Layout = this->CreateDWriteTextLayout(prs[pr].Text,prs[pr].Length);
	this->Paragraphs = prs;
}

void Renderer::CreateDWriteFactory()
{
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		(IUnknown**)(&this->DWriteFactory)
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

ComPtr<ID2D1SolidColorBrush> Renderer::CreateDirect2DBrush(ColorF color)
{
	ComPtr<ID2D1SolidColorBrush> NewDirect2DBrush;
	this->Direct2DTarget->CreateSolidColorBrush(
		color,
		&NewDirect2DBrush
	);
	return NewDirect2DBrush;
}
ComPtr<IDWriteTextLayout> Renderer::CreateDWriteTextLayout(wstring text, UINT textLen, float maxWidth, float maxHeight)
{
	DWRITE_TEXT_RANGE DWriteRange;
	DWriteRange.startPosition = 0;
	DWriteRange.length = textLen;

	ComPtr<IDWriteTextLayout> NewDWriteTextLayout;

	this->DWriteFactory->CreateTextLayout(
		text.c_str(),
		textLen,
		this->DWriteTextFormat.Get(),
		maxWidth,
		maxHeight,
		&NewDWriteTextLayout
	);
	NewDWriteTextLayout->SetFontSize(this->FontSize, DWriteRange);
	return NewDWriteTextLayout;
}
ComPtr<IDWriteTextFormat> Renderer::CreateDWriteTextFormat(float fontSize, wstring fontFamily, DWRITE_FONT_WEIGHT fontWeight, DWRITE_FONT_STYLE fontStyle, DWRITE_FONT_STRETCH fontStretch)
{
	ComPtr<IDWriteTextFormat> NewDWriteTextFormat;
	if (this->DWriteFactory)
	{
		this->DWriteFactory->CreateTextFormat(
			fontFamily.c_str(),
			NULL,
			fontWeight,
			fontStyle,
			fontStretch,
			fontSize,
			L"en-us",
			&NewDWriteTextFormat
		);

		DWRITE_TRIMMING DWriteTrimming;
		DWriteTrimming.delimiter = 0;
		DWriteTrimming.delimiterCount = 0;
		DWriteTrimming.granularity = DWRITE_TRIMMING_GRANULARITY_NONE;

		NewDWriteTextFormat->SetTrimming(&DWriteTrimming, NULL);
		NewDWriteTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
		NewDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		NewDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}
	return NewDWriteTextFormat;
}

void Renderer::RenderTextWithDirect2DContext()
{
	float offsetY = this->ScrollOffset;
	float targetH = this->Direct2DTarget->GetSize().height;

	ID2D1DrawingStateBlock* Direct2DState;
	HRESULT hr = this->Direct2DFactory->CreateDrawingStateBlock(
		&Direct2DState
	);

	if (SUCCEEDED(hr))
		this->Direct2DContext->SaveDrawingState(Direct2DState);
	this->Direct2DContext->BeginDraw();
	this->Direct2DContext->SetTransform(IdentityMatrix());
	this->Direct2DContext->Clear(ColorF(ColorF::DarkSlateGray));
	for (Paragraph pr : this->Paragraphs)
	{
		if (offsetY + this->FontSize >= 0)
		{
			this->Direct2DContext->DrawTextLayout(
				{ 0,offsetY },
				pr.Layout.Get(),
				this->Direct2DBrush.Get());
		}
		offsetY += this->FontSize;
		if (offsetY > targetH)
			break;
	}
	this->Direct2DContext->EndDraw();
	if (SUCCEEDED(hr))
		this->Direct2DContext->RestoreDrawingState(Direct2DState);
}
void Renderer::CreateResources()
{
	this->CreateDWriteFactory();
	this->CreateDirect2DFactory();
	this->DWriteTextFormat = this->CreateDWriteTextFormat(this->FontSize);
}
void Renderer::DiscardResources()
{
	//rewrite
	this->Direct2DBrush->Release();
	this->Direct2DTarget->Release();
}