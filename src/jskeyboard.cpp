#include "jskeyboard.h"
#include "utils.h"

JsKeyboard::JsKeyboard(v8::Isolate* isolate, const Keyboard * const keyboard): isolate(isolate),keyboard(keyboard)
{
}

void JsKeyboard::GetKeyboards(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::HandleScope handle_scope(args.GetIsolate());
    auto keyboards = Keyboard::GetKeyboards();
    auto keyboardsSize = keyboards->size();
    auto keyboardsArray = v8::Array::New(args.GetIsolate(),keyboardsSize);
    size_t i = 0;
    for(auto& keyboard : *keyboards)
    {
        v8::Local<v8::Object> jskeyboard = WrapKeyboard(args.GetIsolate(),&keyboard);
        auto result = keyboardsArray->Set(args.GetIsolate()->GetCurrentContext(), i,jskeyboard);
        if(result.IsJust() && result.ToChecked())
        {
            
        }
        i++;
    }
    
    args.GetReturnValue().Set(keyboardsArray);
}
v8::Local<v8::Object> JsKeyboard::WrapKeyboard(v8::Isolate* isolate, const Keyboard* const keyboard)
{
     v8::EscapableHandleScope handle_scope(isolate);
     JsKeyboard* jsKeyboard = new JsKeyboard(isolate,keyboard);
     v8::Local<v8::ObjectTemplate> keyboardTemplate = v8::ObjectTemplate::New(isolate);
     keyboardTemplate->SetInternalFieldCount(1);
     keyboardTemplate->Set(v8::String::NewFromUtf8(isolate,"ToString",v8::NewStringType::kNormal)
                           .ToLocalChecked(), v8::FunctionTemplate::New(isolate, ToString));

     v8::Local<v8::Object> obj = keyboardTemplate->NewInstance();
     obj->SetInternalField(0, v8::External::New(isolate, jsKeyboard));
     
     return handle_scope.Escape(obj);
}

void JsKeyboard::ToString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> self = args.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));    
    void* ptr = wrap->Value();
    JsKeyboard* jsKeyboard = static_cast<JsKeyboard*>(ptr);    
    args.GetReturnValue().Set(v8::String::NewFromUtf8(args.GetIsolate(), 
                                                      Utils::ToStr(jsKeyboard->keyboard->ToString()).c_str(),
                                                      v8::NewStringType::kNormal).ToLocalChecked());            
    
}
