#include <d2d1.h>
#include <dwrite.h>
#include <d2d1helper.h>

#include <malloc.h>


using namespace D2D1;

class DWTextDrawer
{
	public:
		bool ResourcesCreated;

		DWTextDrawer(HWND window)
		{
			this->Window = window;
			this->CreateResources();
		}
	   ~DWTextDrawer()
		{
		   free(this);
		}

	   void DWDrawText(const wchar_t* text)
	   {
		   GetClientRect(this->Window, &(this->SurfaceRect));
		   D2D1_SIZE_U size = D2D1::SizeU(
			   this->SurfaceRect.right - this->SurfaceRect.left,
			   this->SurfaceRect.bottom - this->SurfaceRect.top
		   );
		   if (SUCCEEDED(Direct2DFactory))
		   {
			   Direct2DFactory->CreateHwndRenderTarget(
				   RenderTargetProperties(),
				   HwndRenderTargetProperties(
					   this->Window,
					   size
				   ),
				   &Direct2DTarget
			   );
		   }
		   if (SUCCEEDED(Direct2DTarget))
		   {
			   Direct2DTarget->CreateSolidColorBrush(
				   ColorF(ColorF::Black),
				   &Direct2DBrush
			   );
		   }
		   Direct2DTarget->BeginDraw();
		   Direct2DTarget->SetTransform(IdentityMatrix());
		   Direct2DTarget->Clear(ColorF(ColorF::White));
		   D2D1_RECT_F layoutRect = RectF(
			   static_cast<FLOAT>(this->SurfaceRect.left),
			   static_cast<FLOAT>(this->SurfaceRect.top),
			   static_cast<FLOAT>(this->SurfaceRect.right - this->SurfaceRect.left),
			   static_cast<FLOAT>(this->SurfaceRect.bottom - this->SurfaceRect.top)
		   );
		   Direct2DTarget->DrawTextW(
			   text,
			   wcslen(text),
			   DWriteTextFormat,
			   layoutRect,
			   Direct2DBrush
		   );
		   Direct2DTarget->EndDraw();
	   }

	private:
		HWND Window;
		RECT SurfaceRect;

		IDWriteFactory* DWriteFactory;
		IDWriteTextFormat* DWriteTextFormat;

		char* Text;
		UINT32 Length;

		ID2D1Factory* Direct2DFactory;
		ID2D1SolidColorBrush* Direct2DBrush;
		ID2D1HwndRenderTarget* Direct2DTarget;

		template <class T> inline void SafeRelease(T* ppT)
		{
			if (ppT)
			{
				ppT->Release();
				ppT = NULL;
			}
		}

		void CreateResources()
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
					L"Lucida",
					NULL,
					DWRITE_FONT_WEIGHT_BOLD,
					DWRITE_FONT_STYLE_NORMAL,
					DWRITE_FONT_STRETCH_NORMAL,
					50.0f,
					L"en-us",
					&DWriteTextFormat
				);
				DWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				DWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			}
		}
		void DiscardResources()
		{
			SafeRelease(Direct2DBrush);
			SafeRelease(Direct2DTarget);
		}
};