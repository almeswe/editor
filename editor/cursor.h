#pragma once

#include <wchar.h>
#include <malloc.h>

#define EOF -1
#define NEW_LINE 10

typedef enum CursorAppeareance
{
	CursorAppeareanceInsert,
	CursorAppeareanceStandart
};
typedef struct CursorPosition
{
	wchar_t* LineText;

	size_t Line;
	size_t Character;
};

class Cursor
{
	public:
		Cursor(CursorPosition* startPosition,CursorAppeareance appeareance = CursorAppeareanceStandart);
	   ~Cursor();

		void MoveCursorForward();
		void MoveCursorBackward();

		void MoveCursorUp();
		void MoveCursorDown();

		CursorPosition* GetPosition();
		CursorAppeareance GetAppeareance();

		void SetAppeareance(CursorAppeareance appeareance);

	private:
		CursorPosition* Position;
		CursorAppeareance Appeareance;
};