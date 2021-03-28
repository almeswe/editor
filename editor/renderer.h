#pragma once

#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>

#include <wrl.h>

#include <vector>
#include <string>

#include "gap.h"

using namespace std;
using namespace D2D1;
using namespace Microsoft::WRL;

#define WHEEL_SCROLL_UP 120
#define WHEEL_SCROLL_DOWN 65416

#define ADDITIONAL_TOP_SCROLLING_OFFSET 0
#define ADDITIONAL_BOTTOM_SCROLLING_OFFSET this->FontSize * ((int)this->Paragraphs.size() - 1)
#define TEXT_SCROLLING_SCALAR this->FontSize*3

#define FONT_SIZE_MIN 7.0f
#define FONT_SIZE_MAX 105.0f
#define FONT_SIZING_SCALAR 7.0f
#define FONT_SIZE_DIFFERENCE (this->FontSize-prevFontSize)

#define START_CONTEXT_DRAWING(drawer) drawer.StartDraw(this->Direct2DContext,this->Direct2DFactory)
#define CLEAR_CONTEXT_SURFACE(drawer) drawer.ClearSurface()
#define END_CONTEXT_DRAWING(drawer)   drawer.EndDraw()

#define TARGET_HEIGHT this->Direct2DTarget->GetSize().height
#define TARGET_WIDTH  this->Direct2DTarget->GetSize().width

#define ADDITIONAL_LEFT_LINECOUNTER_OFFSET this->FontSize * to_wstring(this->Paragraphs.size()).size()

class Renderer
{
	public:
		Renderer(HWND window);
	   ~Renderer();

	   void OnScroll(float delta);
	   void OnCtrlScroll(float delta);
	   void OnResize(UINT width, UINT height);
	   void OnMouseClick(UINT posX,UINT posY,Gap* gap);

	   void RenderText(Gap* gap);
	   void RenderCursor(Gap* gap);

	   void SetParagraphs(vector<Paragraph> prs);

	private:
		HWND Window;
		RECT SurfaceRect;

		wchar_t* Text;
		float FontSize;
		float ScrollOffset;

		vector<Paragraph> Paragraphs;
		
		ID2D1Factory* Direct2DFactory;
		IDWriteFactory* DWriteFactory;
		ID2D1DeviceContext* Direct2DContext;
		ID2D1HwndRenderTarget* Direct2DTarget;
		
		ComPtr<IDWriteTextFormat> DWriteTextFormat;
		ComPtr<ID2D1SolidColorBrush> Direct2DTextBrush;
		ComPtr<ID2D1SolidColorBrush> Direct2DCursorBrush;
		ComPtr<ID2D1SolidColorBrush> Direct2DLineCounterBrush;

		void CreateDWriteFactory();
		void CreateDirect2DTarget();
		void CreateDirect2DFactory();
		void CreateDirect2DContext();

		ComPtr<ID2D1SolidColorBrush> CreateDirect2DBrush(
			ColorF color = ColorF::WhiteSmoke
		);
		ComPtr<IDWriteTextLayout> CreateDWriteTextLayout(
			wstring text,
			UINT textLen,
			float maxWidth = 0,
			float maxHeight = 0
		);
		ComPtr<IDWriteTextFormat> CreateDWriteTextFormat(
			float fontSize,
			wstring fontFamily = L"Consolas", 
			DWRITE_FONT_WEIGHT  fontWeight  = DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE   fontStyle   = DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH_NORMAL
		);

		void RenderTextWithDirect2DContext();

		void CreateResources();
};