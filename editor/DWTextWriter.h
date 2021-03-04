#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include <d2d1helper.h>

#include <malloc.h>

class DWTextDrawer
{
	public:
		bool ResourcesCreated;

		DWTextDrawer(HWND window);
	   ~DWTextDrawer();

	   void DWDrawText(const char* text);
};