#pragma once

#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>

#include <dxgi.h>

class Renderer
{
	public:
		Renderer(HWND window);
	   ~Renderer();

	   void OnResize(UINT width, UINT height);
	   void OnScroll(FLOAT delta);

	   void RenderText();
	   void SetText(wchar_t* text);

	private:
		HWND Window;
		FLOAT FontSize;
		RECT SurfaceRect;

		wchar_t* Text;

		IDWriteFactory* DWriteFactory;
		IDWriteTextFormat* DWriteTextFormat;
		IDWriteTextLayout* DWriteTextLayout;

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