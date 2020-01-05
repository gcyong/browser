#include "application.h"

#include "include/cef_parser.h"

namespace
{
    const unsigned int kBrowserMessageWork{ WM_USER + 100 };
    const int64_t kTimerMaximumMillisec{ 1000 / 30 };

    std::wstring GetProgramPath()
    {
        wchar_t szModulePathBuffer[MAX_PATH + 1]{ 0 };
        GetModuleFileNameW(NULL, szModulePathBuffer, MAX_PATH + 1);

        std::wstring strModulePahtOnly{ szModulePathBuffer };

        auto&& pos{ strModulePahtOnly.find_last_of(L'\\') };
        return strModulePahtOnly.substr(0, pos);
    }

    std::string LoadHTMLDocument(const std::wstring& strFilePath)
    {
        HANDLE hFile{ CreateFileW(
            strFilePath.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL) };

        std::vector<char> vReadBuffer;
        if (hFile != INVALID_HANDLE_VALUE)
        {
            constexpr DWORD kdwReadBlockSize{ 1024 };
            char aReadBlock[kdwReadBlockSize]{ 0 };

            DWORD dwRealReadBlockSize{ 0 };
            while (ReadFile(hFile, aReadBlock, kdwReadBlockSize, &dwRealReadBlockSize, nullptr))
            {
                if (dwRealReadBlockSize > 0)
                {
                    std::copy(aReadBlock, aReadBlock + dwRealReadBlockSize, std::back_inserter(vReadBuffer));
                }
                else
                {
                    break;
                }
            }
        }

        CloseHandle(hFile);

        vReadBuffer.push_back(0);
        return vReadBuffer.data();
    }

    std::string GetDataURI(const std::string& strData, const std::string& strMIMEType)
    {
        std::stringstream ss;
        ss << "data:" << strMIMEType << ";base64,"
            << CefURIEncode(CefBase64Encode(strData.c_str(), strData.size()), false).ToString();

        return ss.str();
    }
}

const std::wstring Application::kApplicationClassName{ L"gcyong_Application" };

Application::Application() :
    mhWnd(NULL),
    mRender(new Render),
    mbTimerStarted(false),
    mbReentered(false),
    mbWorkPerformStarted(false)
{

}

Application::~Application()
{
    mhWnd = NULL;
    mRender = nullptr;
    mBrowser = nullptr;

    mbTimerStarted = false;
    mbReentered = false;
    mbWorkPerformStarted = false;
}

bool Application::Create()
{
    WNDCLASSEX wcex{ 0 };
    if (!GetClassInfoEx(Common::GetInstance(), kApplicationClassName.c_str(), &wcex))
    {
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.cbWndExtra = sizeof(this);
        wcex.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wcex.hInstance = Common::GetInstance();
        wcex.lpfnWndProc = &Application::WndProc;
        wcex.lpszClassName = kApplicationClassName.c_str();
        wcex.style = CS_VREDRAW | CS_HREDRAW;
        RegisterClassEx(&wcex);
    }

    HWND hWnd{ CreateWindowEx(
        0,
        kApplicationClassName.c_str(),
        L"",
        WS_OVERLAPPEDWINDOW,
        0, 0,
        0, 0,
        NULL, NULL, Common::GetInstance(), this) };

    return (IsWindow(hWnd) == TRUE);
}

void Application::Run()
{
    if (ReadyBrowser())
    {
        Resize(800, 600);
        Show();

        MSG msg{ 0 };
        while (static_cast<int>(GetMessage(&msg, NULL, 0, 0)) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        ResetTimer();

        for (int i = 0; i < 10; ++i)
        {
            CefDoMessageLoopWork();
            Sleep(50);
        }
    }
}

void Application::Resize(int width, int height)
{
    if (IsWindow(mhWnd))
    {
        DWORD dwStyle{ static_cast<DWORD>(GetWindowLongPtr(mhWnd, GWL_STYLE)) };
        DWORD dwExStyle{ static_cast<DWORD>(GetWindowLongPtr(mhWnd, GWL_EXSTYLE)) };

        RECT rect{ 0, 0, width, height };
        AdjustWindowRectEx(&rect, dwStyle, IsMenu(GetMenu(mhWnd)), dwExStyle);
        SetWindowPos(mhWnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOREDRAW);

        if (mBrowser != nullptr)
        {
            HWND hWndBrowser{ mBrowser->GetHost()->GetWindowHandle() };
            SetWindowPos(hWndBrowser, NULL, 0, 0, width, height, SWP_NOZORDER);
        }
    }
}

void Application::Show()
{
    ShowWindow(mhWnd, SW_SHOW);
}

void Application::Hide()
{
    ShowWindow(mhWnd, SW_HIDE);
}

CefRefPtr<CefBrowserProcessHandler> Application::GetBrowserProcessHandler()
{
    return this;
}

CefRefPtr<CefRenderProcessHandler> Application::GetRenderProcessHandler()
{
    return mRender;
}

CefRefPtr<CefKeyboardHandler> Application::GetKeyboardHandler()
{
    return this;
}

CefRefPtr<CefDisplayHandler> Application::GetDisplayHandler()
{
    return this;
}

void Application::OnScheduleMessagePumpWork(int64 delay_ms)
{
    PostMessage(mhWnd, kBrowserMessageWork, 0, static_cast<LPARAM>(delay_ms));
}

void Application::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
    if (mBrowser->IsSame(browser))
    {
        SetWindowText(mhWnd, title.c_str());
    }
}

bool Application::OnKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event)
{
    if (os_event->wParam == VK_F12)
    {
        if (mBrowser != nullptr)
        {
            if (mSpy == nullptr)
            {
                mSpy = new Spy(mhWnd, mBrowser);
            }

            mSpy->Show();
            return true;
        }
    }
    else if (os_event->wParam == VK_F5)
    {
        if (mBrowser != nullptr)
        {
            auto&& html{ LoadHTMLDocument(GetProgramPath().append(L"\\test.html")) };
            mBrowser->GetMainFrame()->LoadURL(GetDataURI(html, "text/html"));
            
            return true;
        }
    }

    return false;
}

bool Application::ReadyBrowser()
{
    CefWindowInfo wi;
    wi.SetAsChild(mhWnd, {});

    CefBrowserSettings bs;

    auto&& html{ LoadHTMLDocument(GetProgramPath().append(L"\\test.html")) };
    mBrowser = CefBrowserHost::CreateBrowserSync(wi, this, GetDataURI(html, "text/html"), bs, nullptr, nullptr);

    return (mBrowser != nullptr);
}

void Application::StartTimer(int64_t delay_ms)
{
    if (!mbTimerStarted)
    {
        if (delay_ms > 0)
        {
            mbTimerStarted = true;
            SetTimer(mhWnd, 1, static_cast<UINT>(delay_ms), nullptr);
        }
    }
}

void Application::ResetTimer()
{
    if (mbTimerStarted)
    {
        KillTimer(mhWnd, 1);
        mbTimerStarted = false;
    }
}

void Application::OnBrowserMessageWork(int64 delay_ms)
{
    if (delay_ms == INT32_MAX && mbTimerStarted)
    {
        return;
    }
    else
    {
        ResetTimer();

        if (delay_ms <= 0)
        {
            PerformBrowserWork();
        }
        else
        {
            if (delay_ms > kTimerMaximumMillisec)
            {
                delay_ms = kTimerMaximumMillisec;
            }

            StartTimer(delay_ms);
        }
    }
}

void Application::OnTimer()
{
    ResetTimer();
    PerformBrowserWork();
}

void Application::OnDestroy()
{
    mRender = nullptr;
    mBrowser = nullptr;
    mSpy = nullptr;
    PostQuitMessage(0);
}

LRESULT CALLBACK Application::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_NCCREATE)
    {
        LPCREATESTRUCT pcs{ reinterpret_cast<LPCREATESTRUCT>(lParam) };
        Application* pApp{ reinterpret_cast<Application*>(pcs->lpCreateParams) };

        pApp->mhWnd = hWnd;
        Common::SetDataInto<Application>(hWnd, pApp);
    }
    else
    {
        Application* pApp{ Common::GetDataInfo<Application>(hWnd) };

        switch (uMsg)
        {
        case WM_TIMER :
            pApp->OnTimer();
            return 0;
        case WM_SIZE :
            pApp->Resize(LOWORD(lParam), HIWORD(lParam));
            return 0;
        case WM_DESTROY :
            pApp->OnDestroy();
            return 0;
        case kBrowserMessageWork :
            pApp->OnBrowserMessageWork(static_cast<int64_t>(lParam));
            return 0;
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void Application::PerformBrowserWork()
{
    if (mbWorkPerformStarted)
    {
        mbReentered = true;
    }
    else
    {
        mbReentered = false;
        mbWorkPerformStarted = true;
        {
            CefDoMessageLoopWork();
        }
        mbWorkPerformStarted = false;

        if (mbReentered)
        {
            OnScheduleMessagePumpWork(0);
        }
        else
        {
            if (!mbTimerStarted)
            {
                OnScheduleMessagePumpWork(INT32_MAX);
            }
        }
    }
}