#pragma once

#include "include/cef_client.h"

class Spy : public CefClient
{
public :
    explicit Spy(HWND hWndParent, CefRefPtr<CefBrowser> parentBrowser);
    virtual ~Spy();

    void Show();

private :
    HWND mhWndParent;
    CefRefPtr<CefBrowser> mParentBrowser;

    IMPLEMENT_REFCOUNTING(Spy);
};
