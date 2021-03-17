#pragma once

#include <wchar.h>
#include <string.h>
#include <malloc.h>

#include <wrl.h>
#include <vector>
#include <string>

#include <dwrite.h>

using namespace std;
using namespace Microsoft::WRL;

#define GAP_CH L'\a'
#define NEWLN_CH L'\n'

struct Paragraph
{
	size_t Line;
	size_t Length;

	wstring Text;

	ComPtr<IDWriteTextLayout> Layout;
};

class Gap
{
	public:
		Gap();
	   ~Gap();

	    wchar_t GetAt(size_t pos);
		void RemoveAt(size_t pos);
		void InsertAt(size_t pos, wchar_t ch);
		void InsertAt(size_t pos, const wchar_t* frag);

		void MoveCursorUp();
		void MoveCursorDown();
		void MoveCursorForward();
		void MoveCursorBackward();

		size_t GetCursor();
		wchar_t* GetText();
		vector<Paragraph> GetParagraphs();

	private:
		struct GapCursor
		{
			size_t Position = 0;
			size_t GoalOffset = 0;
		};

		wchar_t* Text;

		size_t GapSize;
		size_t GapR, GapL;

		GapCursor Cursor;

		void MoveGapTo(size_t pos);
		void MoveGapLeft(size_t pos);
		void MoveGapRight(size_t pos);

		void ExtendGap();
		void ExtendText();

		void InsertInText(size_t pos, wchar_t ch);

		void GetGoalOffset();

		wchar_t* WCharAppend(wchar_t* wcharr, wchar_t ch);
};