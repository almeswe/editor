#include "gap.h"

Gap::Gap()
{
	this->Text = new wchar_t[200];
	for (int i = 0; i < 200; i++)
		this->Text[i] = L' ';

	this->GapSize = 25;
	this->GapBuffer = new wchar_t[this->GapSize];

	this->GapL = 0;
	this->GapR = this->GapSize-1;
}

Gap::~Gap()
{

}

wchar_t Gap::GetAt(size_t pos)
{
	return pos < this->GapL ? this->Text[pos] : this->Text[pos + this->GapR - this->GapL];
}

void Gap::Insert(size_t pos,const wchar_t* frag)
{
	if (this->GapL != pos)
		this->MoveGap(pos);

	for (int i = 0; i < wcslen(frag); i++)
	{
		if (this->GapL == this->GapR)
		{
			//!!!
			this->Grow();
		}
		this->Text[pos + i] = frag[i];
		this->GapL++;
	}
}
void Gap::Insert(size_t pos, const wchar_t ch)
{
	if (this->GapL != pos)
		this->MoveGap(pos);
	if (this->GapL == this->GapR)
	{
		//!!!
		this->Grow();
	}
	this->Text[pos] = ch;
	this->GapL++;
}

void Gap::MoveRight(size_t pos)
{
	while (this->GapL != pos)
	{
		this->GapL++;
		this->GapR++;
		this->Text[this->GapL-1] = this->Text[this->GapR];
		this->Text[this->GapR] = L' ';
	}
}
void Gap::MoveLeft(size_t pos)
{
	while (this->GapL != pos)
	{
		this->GapL--;
		this->GapR--;
		this->Text[this->GapR + 1] = this->Text[this->GapL];
		this->Text[this->GapL] = L' ';
	}
}
void Gap::MoveGap(size_t pos)
{
	if (this->GapL > pos)
		this->MoveLeft(pos);
	else
		this->MoveRight(pos);
}
void Gap::Grow()
{

}