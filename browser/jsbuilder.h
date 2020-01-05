#pragma once

#include <string>
#include <functional>
#include <unordered_map>

#include "include/cef_v8.h"
#include "include/cef_values.h"

template <typename T>
class JSBuilder
{
    using FunctionDelegate = CefRefPtr<CefV8Value>(T::*)(const CefV8ValueList&);
public :
    JSBuilder(T* const pHandler, const std::string& strWrapObject) :
        mpHandler(pHandler),
        mstrWrapObject(strWrapObject)
    {
    }

    JSBuilder& Bind(const std::string& strFunctionName, FunctionDelegate functionDelegate)
    {
        mFunctions[strFunctionName] = functionDelegate;
        return *this;
    }

    std::string Build()
    {
        std::stringstream out;
        out << "var " << mstrWrapObject << "; "
            << "if(!" << mstrWrapObject << ") " << mstrWrapObject << " = {}; ";

        out << "(function () { ";
        for (auto&& function : mFunctions)
        {
            out << mstrWrapObject << "." << function.first
                << " = function() { "
                << "native function " << function.first << "(); return " << function.first << "();"
                << "};";
        }
        out << "})();";

        return out.str();
    }

    bool Execute(const std::string& strFunctionName, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retVal)
    {
        if (mFunctions.find(strFunctionName) != mFunctions.end())
        {
            retVal = (mpHandler->*mFunctions[strFunctionName])(arguments);
            return true;
        }

        return false;
    }

private :
    T* const mpHandler;
    std::string mstrWrapObject;
    std::unordered_map<std::string, FunctionDelegate> mFunctions;
};
