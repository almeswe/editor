#include "renderer.h"

struct ContextDrawer
{
	HRESULT hr;
	ID2D1DeviceContext* Direct2DContext;
	ID2D1DrawingStateBlock* Direct2DState;

	void StartDraw(ID2D1DeviceContext* context, ID2D1Factory* factory)
	{
		this->Direct2DContext = context;

		hr = factory->CreateDrawingStateBlock(
			&Direct2DState
		);

		if (SUCCEEDED(hr))
			context->SaveDrawingState(Direct2DState);
		context->BeginDraw();
	}
	void ClearSurface()
	{
		this->Direct2DContext->SetTransform(IdentityMatrix());
		this->Direct2DContext->Clear(ColorF::ColorF(ColorF::DarkSlateBlue));
	}
	void EndDraw()
	{
		this->Direct2DContext->EndDraw();
		if (SUCCEEDED(this->hr))
			this->Direct2DContext->RestoreDrawingState(Direct2DState);
	}
};

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
void Renderer::OnScroll(float delta)
{
	switch ((int)delta)
	{
		case WHEEL_SCROLL_UP:
			this->ScrollOffset += TEXT_SCROLLING_SCALAR;
			break;
		case WHEEL_SCROLL_DOWN:
			this->ScrollOffset -= TEXT_SCROLLING_SCALAR;
			break;
		default:
			return;
	}

	if (this->ScrollOffset > ADDITIONAL_TOP_SCROLLING_OFFSET)
		this->ScrollOffset = ADDITIONAL_TOP_SCROLLING_OFFSET;
	else if (-this->ScrollOffset > ADDITIONAL_BOTTOM_SCROLLING_OFFSET)
		this->ScrollOffset = -ADDITIONAL_BOTTOM_SCROLLING_OFFSET;
}
void Renderer::OnCtrlScroll(float delta)
{
	float prevFontSize = this->FontSize;
	switch ((int)delta)
	{
		case WHEEL_SCROLL_UP:
			this->FontSize += FONT_SIZING_SCALAR;
			break;
		case WHEEL_SCROLL_DOWN:
			this->FontSize -= FONT_SIZING_SCALAR;
			break;
		default:
			return;
	}

	if (this->FontSize > FONT_SIZE_MAX)
		this->FontSize = FONT_SIZE_MAX;
	else if (this->FontSize < FONT_SIZE_MIN)
		this->FontSize = FONT_SIZE_MIN;
	else
	{
		float offset = 0;
		size_t offsetLine = 0;
		while (offset > this->ScrollOffset)
		{
			offsetLine++;
			offset -= prevFontSize;
		}
		this->ScrollOffset -= (FONT_SIZE_DIFFERENCE * offsetLine);

		if (this->ScrollOffset > ADDITIONAL_TOP_SCROLLING_OFFSET)
			this->ScrollOffset = ADDITIONAL_TOP_SCROLLING_OFFSET;
	}
}
void Renderer::OnMouseClick(UINT posX,UINT posY, Gap* gap)
{
	size_t line = (size_t)((-this->ScrollOffset+posY) / this->FontSize);
	if (line >= this->Paragraphs.size())
	{
		line = Paragraphs.size() - 1;
		posY = line * this->FontSize;
	}

	BOOL isTrailing, isInside;
	DWRITE_HIT_TEST_METRICS cursorMetrics;
	this->Paragraphs[line].Layout->HitTestPoint(
		posX,
		posY+this->FontSize,
		&isTrailing,
		&isInside,
		&cursorMetrics
	);

	size_t pos = 0;
	size_t textLen = gap->GetText().size();
	for (int i = 0; i < line; i++)
		pos += this->Paragraphs[i].Length;

	pos += cursorMetrics.textPosition;
	gap->SetCursor(pos);
}

void Renderer::RenderText(Gap* gap)
{
	GetClientRect(this->Window, &this->SurfaceRect);
	if (!this->Direct2DTarget)
	{
		this->CreateDirect2DTarget();
		this->Direct2DTextBrush = this->CreateDirect2DBrush();
		this->Direct2DCursorBrush = this->CreateDirect2DBrush(ColorF::Tomato);
	}
	if (!this->Direct2DContext)
		this->CreateDirect2DContext();

	this->SetParagraphs(gap->GetParagraphs());
	this->RenderTextWithDirect2DContext();
}
void Renderer::RenderCursor(Gap* gap)
{
	float cursorX, cursorY;
	size_t pos = gap->GetPositionInLine();
	size_t line = gap->GetCursorLine() - 1;//not optimized
	float offset = this->ScrollOffset + line * this->FontSize;
	ComPtr<IDWriteTextLayout> prLayout = this->Paragraphs[line].Layout;


	DWRITE_HIT_TEST_METRICS cursorMetrics;
	prLayout->HitTestTextPosition(
		pos,
		0,
		&cursorX,
		&cursorY,
		&cursorMetrics
	);

	ContextDrawer drawer;
	START_CONTEXT_DRAWING(drawer);
	this->Direct2DContext->DrawLine(
		{ cursorX,offset + cursorY },
		{ cursorX,offset + cursorY + cursorMetrics.height },
		this->Direct2DCursorBrush.Get(),
		this->FontSize / 13
	);
	END_CONTEXT_DRAWING(drawer);
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
	DWRITE_TEXT_RANGE DWriteRange {
		0,
		textLen
	};
	
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

		DWRITE_TRIMMING DWriteTrimming {
			DWRITE_TRIMMING_GRANULARITY_NONE,
			0,
			0
		};
		NewDWriteTextFormat->SetTrimming(&DWriteTrimming, NULL);
		NewDWriteTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
		NewDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		NewDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}
	return NewDWriteTextFormat;
}

void Renderer::RenderTextWithDirect2DContext()
{
	float offset = this->ScrollOffset;

	ContextDrawer drawer;
	START_CONTEXT_DRAWING(drawer);
	CLEAR_CONTEXT_SURFACE(drawer);
	for (Paragraph pr : this->Paragraphs)
	{
		if (offset + this->FontSize >= 0)
		{
			this->Direct2DContext->DrawTextLayout(
				{ 0,offset },
				pr.Layout.Get(),
				this->Direct2DTextBrush.Get());
		}
		offset += this->FontSize;
		if (offset > TARGET_HEIGHT)
			break;
	}
	END_CONTEXT_DRAWING(drawer);
}
void Renderer::CreateResources()
{
	this->CreateDWriteFactory();
	this->CreateDirect2DFactory();
	this->DWriteTextFormat = this->CreateDWriteTextFormat(this->FontSize,L"Consolas");
}
void Renderer::DiscardResources()
{
	//rewrite
	this->Direct2DTextBrush->Release();
	this->Direct2DTarget->Release();
}