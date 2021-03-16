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

class Renderer
{
	public:
		Renderer(HWND window);
	   ~Renderer();

	   void OnScroll(FLOAT delta);
	   void OnCtrlScroll(FLOAT delta);
	   void OnResize(UINT width, UINT height);

	   void RenderText();
	   void RenderCursor(size_t pos);

	   void SetText(wchar_t* text);
	   void SetParagraphs(vector<Paragraph> prs);

	   //TEST
	   void RenderText(Gap* gap);

	private:
		HWND Window;
		FLOAT FontSize;
		RECT SurfaceRect;

		wchar_t* Text;

		//TEST
		int CurrentFirstLine;
		FLOAT ScrollOffset;
		vector<Paragraph> Paragraphs;

		IDWriteFactory* DWriteFactory;
		IDWriteTextFormat* DWriteTextFormat;
		ComPtr<IDWriteTextLayout> DWriteTextLayout;

		ID2D1Factory* Direct2DFactory;
		ID2D1DeviceContext* Direct2DContext;
		ID2D1SolidColorBrush* Direct2DBrush;
		ID2D1HwndRenderTarget* Direct2DTarget;

		void CreateDWriteFactory();
		void CreateDWriteTextFormat();
		void CreateDWriteTextLayout();

		void CreateDirect2DBrush();
		void CreateDirect2DTarget();
		void CreateDirect2DFactory();
		void CreateDirect2DContext();

		void RenderTextWithDirect2DContext();
		void RenderTextWithDirect2DTarget();

		void CreateResources();
		void DiscardResources();

};