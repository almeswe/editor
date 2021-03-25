#include "clipboard.h"

void Clipboard::SetMainHandle(HWND handle)
{
	this->Handle = handle;
}
HRESULT Clipboard::ReadData()
{
	if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
        return S_FALSE;
	if (!SUCCEEDED(this->Open()))
		return S_FALSE;//add this->Close
	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (!hData)
	{
		this->Close();
		return S_FALSE;
	}
	else
	{
		size_t len = GlobalSize(hData);
		this->Text = static_cast<wchar_t*>(GlobalLock(hData));
		GlobalUnlock(hData);
	}
	this->Close();
	return S_OK;
}
HRESULT Clipboard::Open()
{
	if (!IsOpened)
	{
		if (!this->Handle)
			return S_FALSE;
		if (!OpenClipboard(this->Handle))
			return S_FALSE;
		this->IsOpened = true;
		return S_OK;
	}
	else
		return S_FALSE;
}
HRESULT Clipboard::Close()
{
	if (IsOpened)
	{
		if (!CloseClipboard())
			return S_FALSE;
		this->IsOpened = false;
		return S_OK;
	}
	else
		return S_FALSE;
}