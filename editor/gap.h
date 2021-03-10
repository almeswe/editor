#pragma once

#include <wchar.h>
#include <string.h>
#include <malloc.h>

#define GAP_CH L'\a'

class Gap
{
	public:
		Gap();
	   ~Gap();

	    wchar_t GetAt(size_t pos);
		void RemoveAt(size_t pos);
		void InsertAt(size_t pos, wchar_t ch);
		void InsertAt(size_t pos, const wchar_t* frag);

		size_t GetPoint();
		wchar_t* GetText();

		void MovePointForward();
		void MovePointBackward();

		wchar_t* GetLine(size_t index);

	private:
		wchar_t* Text;
		
		size_t Point;
		size_t GapSize;
		size_t GapR, GapL;

		void MoveGap(size_t pos);
		void MoveLeft(size_t pos);
		void MoveRight(size_t pos);

		void ExtendGap();
		void ExtendText();

		void InsertInText(size_t pos, wchar_t ch);

		wchar_t* WCharAppend(wchar_t* wcharr, wchar_t ch);
};