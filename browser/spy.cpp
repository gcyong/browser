#include "spy.h"

Spy::Spy(HWND hWndParent, CefRefPtr<CefBrowser> parentBrowser) :
    mhWndParent(hWndParent), mParentBrowser(parentBrowser)
{

}

Spy::~Spy()
{
    if (mParentBrowser != nullptr)
    {
        if (mParentBrowser->GetHost()->HasDevTools())
        {
            mParentBrowser->GetHost()->CloseDevTools();
        }

        mParentBrowser = nullptr;
    }
}

void Spy::Show()
{
    if (mParentBrowser != nullptr)
    {
        if (!mParentBrowser->GetHost()->HasDevTools())
        {
            HWND hWndParent{ mParentBrowser->GetHost()->GetWindowHandle() };

            CefWindowInfo wi;
            wi.SetAsPopup(mhWndParent, L"Browser Spy");

            CefBrowserSettings bs;

            mParentBrowser->GetHost()->ShowDevTools(wi, this, bs, {});
        }
    }
}