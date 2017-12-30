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
    static v8::Local<v8::Object> WrapKeyboard(v8::Isolate* isolate, 
                                              const Keyboard* const keyboard);
private:
    v8::Isolate* isolate;
    const Keyboard* const keyboard;
};

#endif // JSKEYBOARD_H
