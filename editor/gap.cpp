#include "gap.h"

Gap::Gap()
{
	this->GapL = 0;
	this->GapR = 0;
	this->GapSize = 150;

	this->Text = GAP_CH;
}
Gap::~Gap()
{
	//free(this->Text);
}

wchar_t Gap::GetAt(size_t pos)
{
	return pos < this->GapL ? this->Text[pos] : this->Text[pos + this->GapR - this->GapL];
}
void Gap::RemoveAt(size_t pos)
{
	if (pos < 1)
		return;
	if (this->GapL != pos)
		this->MoveGapTo(pos);
	while (pos > 0)
	{
		this->GapL--;
		this->MoveCursorBackward();
		if (this->Text[pos - 1] != GAP_CH)
		{
			this->Text[pos-1] = GAP_CH;
			break;
		}
		else 
			pos--;
	}
}
void Gap::InsertAt(size_t pos, wchar_t ch)
{
	if (this->GapL != pos)
		this->MoveGapTo(pos);
	if (this->GapL == this->GapR)
		this->ExtendGap();
	this->GapL++;
	this->InsertInText(pos, ch);
	this->MoveCursorForward();
}
void Gap::InsertAt(size_t pos, const wchar_t* frag)
{
	if (this->GapL != pos)
		this->MoveGapTo(pos);
	for (size_t i = 0; i < wcslen(frag); i++)
	{
		if (this->GapL == this->GapR)
			this->ExtendGap();
		this->GapL++;
		this->InsertInText(pos+i, frag[i]);
		this->MoveCursorForward();
	}
}

size_t Gap::GetCursorLine()
{
	size_t textPos = 0;
	size_t pos = this->Cursor.Position;
	vector<Paragraph> prs = this->GetParagraphs();

	for (Paragraph pr : prs)
	{
		if (textPos + pr.Length > pos)
			return pr.Line;
		textPos += pr.Length;
	}
	return prs.size() - 1;
}
size_t Gap::GetCursorPosition()
{
	return this->Cursor.Position;
}
wstring Gap::GetText()
{
	wstring newText = L"";
	for (int i = 0; i < this->Text.size(); i++)
		if (this->Text[i] != GAP_CH)
			newText.append(&this->Text[i]);
	return newText;
}
vector<Paragraph> Gap::GetParagraphs()
{
	size_t line = 0;
	size_t length = 1;
	size_t startsAt = 0;
	Paragraph currentPr;
	vector<Paragraph> prs;
	wstring currentLine = L"";

	size_t textlen = this->Text.size();

	for (size_t i = 0; i < textlen; i++)
	{
		if (this->Text[i] == NEWLN_CH || i == textlen - 1)
		{
			//wtf
			if (i == textlen - 1)
			{
				length++;
				currentLine += this->Text[i];
			}
			//
			prs.push_back(CreateParagraph(
				line,
				length,
				currentLine,
				startsAt - (length - 1)
			));

			line++;
			startsAt++;
			length = 1;
			currentLine = L"";
		}
		else
		{
			if (this->Text[i] != GAP_CH)
			{
				length++;
				startsAt++;
				currentLine += this->Text[i];
			}
		}
	}
	return prs;
}
Paragraph Gap::CreateParagraph(size_t line, size_t length, wstring text, size_t startsAt)
{
	Paragraph pr;
	pr.Text = text;
	pr.Line = line;
	pr.Length = length;
	pr.StartsAt = startsAt;
	return pr;
}
size_t Gap::GetPositionInLine()
{
	size_t textPos = 0;
	size_t pos = this->Cursor.Position;
	for (Paragraph pr : this->GetParagraphs())
	{
		if (textPos + pr.Length > pos)
			return pos - textPos;
		textPos += pr.Length;
	}
	//??
	return pos;
}
void Gap::SetCursor(size_t pos)
{
	if (this->Text.size() >= pos)
	{
		this->Cursor.Position = pos;
		this->MoveGapTo(pos);
	}
}
void Gap::MoveCursorUp()
{
	size_t startLineLen = 0;
	//while-loop which calculates it, loses one increment (idk why)
	size_t upperLineLen = 1;
	size_t startLineOffset = 0;
	size_t upperLineOffset = 0;

	size_t currentPos = this->Cursor.Position;

	while (currentPos > 0 && this->Text[currentPos-1] != NEWLN_CH)
	{
		startLineLen++;
		currentPos--;
	}
	startLineOffset = currentPos;

	if (currentPos > 2)
	{
		//to skip offset char of 1-st string and \n after it
		currentPos -= 2;

		while (currentPos > 0 && this->Text[currentPos] != NEWLN_CH)
		{
			upperLineLen++;
			currentPos--;
		}
		upperLineOffset = currentPos;

		if (this->Cursor.GoalOffset >= upperLineLen)
			this->Cursor.Position = upperLineOffset + upperLineLen;
		else
		{
			//??
			if (currentPos != 0)
				upperLineOffset++;
			this->Cursor.Position = upperLineOffset + this->Cursor.GoalOffset;
		}
		this->MoveGapTo(this->Cursor.Position);
	}
	else
	{
		if (currentPos > 0)
		{
			//if the upperline first in the text and contains only '\n' (or other control char)
			this->Cursor.Position = currentPos - 1;
			this->MoveGapTo(this->Cursor.Position);
		}
	}
}
void Gap::MoveCursorDown()
{
	size_t startLineLen = 0;
	size_t bottomLineLen = 0;
	size_t startLineOffset = 0;
	size_t bottomLineOffset = 0;

	size_t gapChars = 0;
	size_t len = this->Text.size() - 1;
	size_t currentPos = this->Cursor.Position;

	//first need to determine the offset of start string:
	while (currentPos > 0 && this->Text[currentPos - 1] != NEWLN_CH)
		currentPos--;
	startLineOffset = currentPos;

	//and then calculate length
	while (currentPos <= len && this->Text[currentPos] != NEWLN_CH)
	{
		//this construction need for counting gap-chars and remove the offset in future
		//gap-chars will met because the currentPos always increases
		if (this->Text[currentPos] != GAP_CH)
			startLineLen++;
		else
			gapChars++;
		currentPos++;
	}

	if (currentPos <= len)
	{
		currentPos++;
		bottomLineOffset = currentPos;

		while (currentPos <= len && this->Text[currentPos] != NEWLN_CH)
		{
			if (this->Text[currentPos] != GAP_CH)
				bottomLineLen++;
			currentPos++;
		}

		if (this->Cursor.GoalOffset >= bottomLineLen)
			this->Cursor.Position = bottomLineOffset + bottomLineLen - gapChars;
		else
			this->Cursor.Position = bottomLineOffset + this->Cursor.GoalOffset - gapChars;
		this->MoveGapTo(this->Cursor.Position);
	}
}
void Gap::MoveCursorForward()
{
	bool forwardIsEmpty = true;
	for (size_t i = this->Cursor.Position; i < this->Text.size(); i++)
		if (this->Text[i] != GAP_CH)
		{
			forwardIsEmpty = false;
			break;
		}
	if (this->Cursor.Position + 1 < this->Text.size())
		if (!forwardIsEmpty)
			this->Cursor.Position++;
	this->GetGoalOffset();
	this->MoveGapRight(this->Cursor.Position);
}
void Gap::MoveCursorBackward()
{
	if (this->Cursor.Position >= 1)
		this->Cursor.Position--;
	this->GetGoalOffset();
	this->MoveGapLeft(this->Cursor.Position);
}
void Gap::MoveGapTo(size_t pos)
{
	if (this->GapL > pos)
		this->MoveGapLeft(pos);
	else
		this->MoveGapRight(pos);
}
void Gap::MoveGapLeft(size_t pos)
{
	while (this->GapL != pos)
	{
		this->GapL--;
		this->GapR--;
		this->Text[this->GapR + 1] = this->Text[this->GapL];
		this->Text[this->GapL] = GAP_CH;
	}
}
void Gap::MoveGapRight(size_t pos)
{
	while (this->GapL != pos)
	{
		this->GapL++;
		if (GapR < this->Text.size())
		{
			this->GapR++;
			this->Text[this->GapL - 1] = this->Text[this->GapR];
			this->Text[this->GapR] = GAP_CH;
		}
	}
}
void Gap::ExtendGap()
{
	size_t pos = this->GapL;

	wstring forwtext = L"";
	size_t  forwlen = this->Text.size() - pos - 1;

	for (size_t i = 0; i < forwlen; i++)
		forwtext.append(&this->Text[pos + i + 1]);

	for (size_t i = 0; i < this->GapSize; i++)
	{
		this->GapR++;
		this->InsertInText(GapR, GAP_CH);
	}

	for (size_t i = 0; i < forwlen; i++)
		this->InsertInText(i+GapR+1, forwtext[i]);
}
void Gap::InsertInText(size_t pos, wchar_t ch)
{
	if (pos >= this->Text.size())
		this->Text.append(&ch);
	else
		this->Text[pos] = ch;
}
void Gap::GetGoalOffset()
{
	size_t offset = 0;
	size_t currentPos = this->Cursor.Position;
	while (currentPos > 0 && this->Text[currentPos-1] != NEWLN_CH)
	{
		offset++;
		currentPos--;
	}
	this->Cursor.GoalOffset = offset;
}