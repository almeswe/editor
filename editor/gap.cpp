#include "gap.h"

Gap::Gap()
{
	this->Point = 0;
	this->GapSize = 5;

	this->GapL = 0;
	this->GapR = 0;

	this->Text = new wchar_t[1];
	this->Text[0] = GAP_CH;
	this->Text[1] = L'\0';
}
Gap::~Gap()
{
	free(this->Text);
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
		this->MoveGap(pos);

	this->Text[pos - 1] = GAP_CH;
	this->GapL--;
	this->Point--;
}
void Gap::InsertAt(size_t pos, wchar_t ch)
{
	if (this->GapL != pos)
		this->MoveGap(pos);
	if (this->GapL == this->GapR)
		this->ExtendGap();

	this->InsertInText(pos, ch);
	this->GapL++;
	this->Point++;
}
void Gap::InsertAt(size_t pos, const wchar_t* frag)
{
	if (this->GapL != pos)
		this->MoveGap(pos);
	for (int i = 0; i < wcslen(frag); i++)
	{
		if (this->GapL == this->GapR)
			this->ExtendGap();

		this->InsertInText(pos+i, frag[i]);
		this->GapL++;
		this->Point++;
	}
}

size_t Gap::GetPoint()
{
	return this->Point;
}
wchar_t* Gap::GetText()
{
	return this->Text;
}

void Gap::MovePointForward()
{
	if (this->Point + 1 < wcslen(this->Text))
	{
		this->Point++;
		this->MoveRight(this->Point);
	}
}
void Gap::MovePointBackward()
{
	if (this->Point >= 1)
	{
		this->Point--;
		this->MoveLeft(this->Point);
	}
}

void Gap::MoveGap(size_t pos)
{
	if (this->GapL > pos)
		this->MoveLeft(pos);
	else
		this->MoveRight(pos);
}
void Gap::MoveLeft(size_t pos)
{
	while (this->GapL != pos)
	{
		this->GapL--;
		this->GapR--;
		this->Text[this->GapR + 1] = this->Text[this->GapL];
		this->Text[this->GapL] = GAP_CH;
	}
}
void Gap::MoveRight(size_t pos)
{
	while (this->GapL != pos)
	{
		this->GapL++;
		this->GapR++;
		if (GapR < wcslen(this->Text))
		{
			this->Text[this->GapL - 1] = this->Text[this->GapR];
			this->Text[this->GapR] = GAP_CH;
		}
		else
			GapR--;
	}
}

void Gap::ExtendGap()
{
	size_t pos = this->GapL;
	size_t forwlen = wcslen(this->Text) - pos - 1;
	
	wchar_t* forwtext = new wchar_t[forwlen];
	for (int i = 0; i < forwlen; i++)
		forwtext[i] = this->Text[pos + i + 1];

	for (int i = 0; i < this->GapSize; i++)
	{
		this->GapR++;
		this->InsertInText(GapR, GAP_CH);
	}

	for (int i = 0; i < forwlen; i++)
		this->InsertInText(i+GapR+1, forwtext[i]);
}
void Gap::ExtendText()
{
	size_t len = wcslen(this->Text);
	size_t nlen = len + 1;
	wchar_t* exttext = new wchar_t[nlen];
	for (int i = 0; i < len; i++)
		exttext[i] = this->Text[i];
	exttext[nlen] = L'\0';
	exttext[nlen-1] = GAP_CH;
	this->Text = exttext;
}

void Gap::InsertInText(size_t pos, wchar_t ch)
{
	if (pos >= wcslen(this->Text))
		this->ExtendText();
	this->Text[pos] = ch;
}