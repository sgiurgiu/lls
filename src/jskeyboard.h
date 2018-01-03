#ifndef JSKEYBOARD_H
#define JSKEYBOARD_H

#include "keyboard.h"

#include <v8.h>

class JsKeyboard
{
public:
    JsKeyboard(v8::Isolate*, const Keyboard* const);
    static void GetKeyboards(const v8::FunctionCallbackInfo<v8::Value>& args);
private:
    static void ToString(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SetAllKeys(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetAllKeys(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void UpdateKeys(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Local<v8::Object> WrapKeyboard(v8::Isolate* isolate, 
                                              const Keyboard* const keyboard);
    static v8::Local<v8::Object> WrapKey(v8::Isolate* isolate,const Key& key);
    static void GetKeyName(v8::Local<v8::String> property,const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetKeyCode(v8::Local<v8::String> property,const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetKeyColor(v8::Local<v8::String> property,const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetKeyColor(v8::Local<v8::String> property, v8::Local<v8::Value> value,const v8::PropertyCallbackInfo<void>& info);
private:
    v8::Isolate* isolate;
    const Keyboard* const keyboard;
};

#endif // JSKEYBOARD_H
