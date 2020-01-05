#pragma once

#include <string>
#include <Windows.h>
#include <Shlwapi.h>

#include "spy.h"
#include "common.h"
#include "render.h"

#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/cef_browser_process_handler.h"

class Application :
    public CefApp,
    public CefClient,
    public CefBrowserProcessHandler,
    public CefDisplayHandler,
    public CefKeyboardHandler
{
    static const std::wstring kApplicationClassName;

public :
    explicit Application();
    virtual ~Application();

    bool Create();
    void Run();

protected :
    // CefApp
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;
    virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() override;
    
    // CefClient
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override;

    // CefBrowserProcessHandler
    virtual void OnScheduleMessagePumpWork(int64 delay_ms) override;

    // CefDisplayHandler
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override;

    // CefKeyboardHandler
    virtual bool OnKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event) override;

protected :
    bool ReadyBrowser();

    void Resize(int width, int height);
    void Show();
    void Hide();

    void StartTimer(int64_t delay_ms);
    void ResetTimer();

    void OnBrowserMessageWork(int64 delay_ms);
    void OnTimer();
    void OnDestroy();

    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

    void PerformBrowserWork();

private :
    HWND mhWnd;

    CefRefPtr<Spy> mSpy;
    CefRefPtr<Render> mRender;
    CefRefPtr<CefBrowser> mBrowser;

    bool mbTimerStarted;
    bool mbReentered;
    bool mbWorkPerformStarted;

    IMPLEMENT_REFCOUNTING(Application);
};