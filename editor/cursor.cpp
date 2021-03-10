#include "cursor.h"

Cursor::Cursor(CursorPosition* startPosition, CursorAppeareance appeareance)
{
	this->Position = startPosition;
	this->Appeareance = appeareance;
}
Cursor::~Cursor()
{
	free(this->Position);
}
void Cursor::MoveCursorForward()
{
	size_t position = this->Position->Character;

	switch (this->Position->LineText[position+1])
	{
		case EOF:
			break;

		case NEW_LINE:
			//TODO change LineText
			this->Position->Character = 0;
			this->Position->Line++;
			break;
		
		default:
			this->Position->Character++;
			break;
	}
}
void Cursor::MoveCursorBackward()
{
	size_t position = this->Position->Character;

	if (position - 1 < 0)
		return;

	switch (this->Position->LineText[position - 1])
	{
		case NEW_LINE:
			//TODO change LineText
			this->Position->Character = wcslen(this->Position->LineText);
			this->Position->Line--;
			break;

		default:
			this->Position->Character--;
			break;
	}
}
void Cursor::MoveCursorUp()
{
}
void Cursor::MoveCursorDown()
{
	
}
CursorPosition* Cursor::GetPosition()
{
	return this->Position;
}
CursorAppeareance Cursor::GetAppeareance()
{
	return this->Appeareance;
}
void Cursor::SetAppeareance(CursorAppeareance appeareance)
{
	this->Appeareance = appeareance;
}