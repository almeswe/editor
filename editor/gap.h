#pragma once
#include <string.h>


class Gap
{
	public:
		wchar_t* Text;

		wchar_t GetAt(size_t pos);
		void Insert(size_t pos, wchar_t ch);
		void Insert(size_t pos,const wchar_t* frag);
		void RemoveAt(size_t pos);

		Gap();
	   ~Gap();

	private:
		
		size_t GapSize;
		size_t GapR, GapL;
		wchar_t* GapBuffer;

		void MoveLeft(size_t pos);
		void MoveRight(size_t pos);
		void MoveGap(size_t pos);

		void Grow();
};

