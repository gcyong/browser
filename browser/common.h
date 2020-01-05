#pragma once

#include <Windows.h>


namespace Common
{
    template <typename DataType = void>
    inline void SetDataInto(HWND hWnd, DataType* pData)
    {
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pData));
    }

    template <typename DataType>
    inline DataType* GetDataInfo(HWND hWnd)
    {
        return reinterpret_cast<DataType*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    HINSTANCE GetInstance();
}