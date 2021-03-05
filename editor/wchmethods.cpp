#pragma ignore();
#include "wchmethods.h"

wchar_t* append(wchar_t* arr, wchar_t ch)
{
    size_t len = wcslen(arr);
    wchar_t* arr2 = new wchar_t[len + 2];

    wcscpy_s(arr2, len + 2, arr);
    arr2[len] = ch;
    arr2[len + 1] = L'\0';

    return arr2;
}

wchar_t* insert(wchar_t* arr, wchar_t ch,size_t pos)
{
    return arr;
}

wchar_t* remlst(wchar_t* arr)
{
    size_t len = wcslen(arr);
    if (len <= 0)
        return arr;
    arr[len - 1] = L'\0';
    while (len >= 0)
    {
        if (arr[len] != L'\0')
        {
            arr[len] = L'\0';
            break;
        }
        len--;
    }
    return arr;
}