#pragma once

#include <memory>

#include "jsbuilder.h"

#include "include/cef_render_process_handler.h"

class Render :
    public CefRenderProcessHandler,
    public CefV8Handler
{
public :
    explicit Render();
    virtual ~Render();

    // CefRenderProcessHandler
    virtual void OnWebKitInitialized() override;

    // CefV8Handler
    virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) override;

    // Functions
    //CefRefPtr<CefV8Value> Foo(const CefV8ValueList& arguments);

private :
    JSBuilder<Render> mBuilder;

    IMPLEMENT_REFCOUNTING(Render);
};