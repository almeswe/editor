#pragma once

#include <wchar.h>
#include <malloc.h>

#include <wrl.h>
#include <vector>
#include <string>
#include <dwrite.h>

using namespace std;
using namespace Microsoft::WRL;

#define GAP_CH L'\a'
#define NEWLN_CH L'\n'
#define SPACE_CH L' '

struct Paragraph
{
	size_t Line;
	size_t Length;
	size_t StartsAt;

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

		wstring GetText();
		size_t GetCursorLine();
		size_t GetCursorPosition();
		size_t GetPositionInLine();
		vector<Paragraph> GetParagraphs();

		void SetCursor(size_t pos);

	private:
		struct GapCursor
		{
			size_t Position = 0;
			size_t GoalOffset = 0;
		};

		wstring Text;

		size_t GapSize;
		size_t GapR, GapL;

		GapCursor Cursor;

		void MoveGapTo(size_t pos);
		void MoveGapLeft(size_t pos);
		void MoveGapRight(size_t pos);

		void ExtendGap();
		void GetGoalOffset();
		void InsertInText(size_t pos, wchar_t ch);

		Paragraph CreateParagraph(size_t line, size_t length, wstring text, size_t startsAt);
};