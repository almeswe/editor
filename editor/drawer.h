#pragma once

#include <d2d1.h>
#include <dwrite.h>

class DWTextDrawer
{
	public:
		DWTextDrawer(HWND window);
	   ~DWTextDrawer();

	   //onscroll
	   //onresize
	   void OnResize(UINT width, UINT height);
	   void OnScroll(FLOAT delta);

	   void DWDrawText(const wchar_t* text);

	private:
		HWND Window;
		RECT SurfaceRect;

		IDWriteFactory* DWriteFactory;
		IDWriteTextFormat* DWriteTextFormat;

		ID2D1Factory* Direct2DFactory;
		ID2D1SolidColorBrush* Direct2DBrush;
		ID2D1HwndRenderTarget* Direct2DTarget;

		//TODO add all text's options
		FLOAT FontSize;

		void CreateResources();
		void DiscardResources();
};