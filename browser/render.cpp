#include "render.h"

Render::Render() : mBuilder(this, "test")
{
    //mBuilder.Bind("hello", &Render::Hello);
}

Render::~Render() = default;

void Render::OnWebKitInitialized()
{
    CefRegisterExtension(L"v8/test", mBuilder.Build(), this);   
}

bool Render::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception)
{
    return mBuilder.Execute(name, arguments, retval);
}