#include "common.h"

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace Common
{
    HINSTANCE GetInstance()
    {
        return reinterpret_cast<HINSTANCE>(&__ImageBase);
    }
}