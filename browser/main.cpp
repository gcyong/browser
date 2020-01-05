#include "common.h"
#include "application.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pszCmdLine, int nCmdShow)
{
    CefEnableHighDPISupport();

    CefMainArgs args{ hInstance };
    CefRefPtr<Application> application{ new Application };

    int exit{ CefExecuteProcess(args, application, nullptr) };
    if (exit >= 0)
    {
        return exit;
    }

    if (application->Create())
    {
        CefSettings settings;
        settings.external_message_pump = 1;
        if (CefInitialize(args, settings, application, nullptr))
        {
            application->Run();
        }
    }

    application = nullptr;
    CefShutdown();
    return 0;
}