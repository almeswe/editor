#pragma once

#include <string>
#include <iostream>
#include <ostream>
#include <windows.h>

using namespace std;

struct Clipboard
{
	wchar_t* Text;

	HRESULT ReadData();
	HRESULT SetData(wstring data);

	void SetMainHandle(HWND handle);
	
	private:
		HWND Handle;
		bool IsOpened = false;

		HRESULT Open();
		HRESULT Close();
};