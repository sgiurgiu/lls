#ifndef GLOBALS_H
#define GLOBALS_H

#include <v8.h>

class Globals
{
private:
    Globals(){}
public:
    static void Version(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Log(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Sleep(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Require(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetNum(v8::Local<v8::String> property,const v8::PropertyCallbackInfo<v8::Value>& info);
    
};

#endif // GLOBALS_H
