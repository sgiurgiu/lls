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
    keyboardTemplate->Set(v8::String::NewFromUtf8(isolate,"SetAllKeys",v8::NewStringType::kNormal)
                        .ToLocalChecked(), v8::FunctionTemplate::New(isolate, SetAllKeys));
    keyboardTemplate->Set(v8::String::NewFromUtf8(isolate,"GetKeys",v8::NewStringType::kNormal)
                        .ToLocalChecked(), v8::FunctionTemplate::New(isolate, GetAllKeys));
    v8::Local<v8::Object> obj = keyboardTemplate->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
    obj->SetInternalField(0, v8::External::New(isolate, jsKeyboard));
    return handle_scope.Escape(obj);
}

void JsKeyboard::SetKeysColors(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    
}

void JsKeyboard::GetAllKeys(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::HandleScope handle_scope(args.GetIsolate());
    v8::Local<v8::Object> self = args.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));    
    void* ptr = wrap->Value();
    JsKeyboard* jsKeyboard = static_cast<JsKeyboard*>(ptr);
    auto allKeys = jsKeyboard->keyboard->GetAllKeys();
    auto keysSize = allKeys.size();
    auto keysArray = v8::Array::New(args.GetIsolate(),keysSize);
    size_t i = 0;
    for(const auto& key : allKeys)
    {
        v8::Local<v8::Object> jskey = WrapKey(args.GetIsolate(),key);
        auto result = keysArray->Set(args.GetIsolate()->GetCurrentContext(), i, jskey);
        if(result.IsNothing())
        {
            
        }
        i++;
    }
    args.GetReturnValue().Set(keysArray);
}
void JsKeyboard::GetKeyName(v8::Local<v8::String> property,const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::HandleScope handle_scope(info.GetIsolate());
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    Key* key = static_cast<Key*>(ptr);    
    info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), 
                                                      key->name.c_str(),
                                                      v8::NewStringType::kNormal).ToLocalChecked());
}
void JsKeyboard::GetKeyCode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::HandleScope handle_scope(info.GetIsolate());
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    Key* key = static_cast<Key*>(ptr);
    info.GetReturnValue().Set(v8::Integer::NewFromUnsigned(info.GetIsolate(),(uint32_t)key->code));
}
void JsKeyboard::GetKeyColor(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::HandleScope handle_scope(info.GetIsolate());
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    Key* key = static_cast<Key*>(ptr);
    info.GetReturnValue().Set(v8::Integer::NewFromUnsigned(info.GetIsolate(),key->color));
}
void JsKeyboard::SetKeyColor(v8::Local<v8::String> property, v8::Local<v8::Value> value,
                            const v8::PropertyCallbackInfo<void>& info)
{
    v8::HandleScope handle_scope(info.GetIsolate());
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    Key* key = static_cast<Key*>(ptr);
    if(!value.IsEmpty() && value->IsUint32())
    {
        key->color = value->ToUint32(info.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Value();
    }
}
v8::Local<v8::Object> JsKeyboard::WrapKey(v8::Isolate* isolate, const Key& key)
{
    v8::EscapableHandleScope handle_scope(isolate);
    v8::Local<v8::ObjectTemplate> keyTemplate = v8::ObjectTemplate::New(isolate);
    keyTemplate->SetInternalFieldCount(1);
    keyTemplate->SetAccessor(v8::String::NewFromUtf8(isolate,"name",v8::NewStringType::kNormal)
                        .ToLocalChecked(),GetKeyName);
    keyTemplate->SetAccessor(v8::String::NewFromUtf8(isolate,"code",v8::NewStringType::kNormal)
                        .ToLocalChecked(),GetKeyCode);
    keyTemplate->SetAccessor(v8::String::NewFromUtf8(isolate,"color",v8::NewStringType::kNormal)
                        .ToLocalChecked(),GetKeyColor,SetKeyColor);
    v8::Local<v8::Object> obj = keyTemplate->NewInstance();
    obj->SetInternalField(0, v8::External::New(isolate, new Key(key.code,key.name,key.color)));
    return handle_scope.Escape(obj);
}

void JsKeyboard::SetAllKeys(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::HandleScope handle_scope(args.GetIsolate());
    v8::Local<v8::Object> self = args.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    JsKeyboard* jsKeyboard = static_cast<JsKeyboard*>(ptr);
    auto argsLength = args.Length();
    uint32_t color = 0;
    if(argsLength > 0 && args[0]->IsInt32())
    {
        auto maybeLocalRed = args[0]->ToInt32(args.GetIsolate()->GetCurrentContext());
        if(!maybeLocalRed.IsEmpty())
        {
            auto red = maybeLocalRed.ToLocalChecked()->Value();
            if(red>=0 && red <= 255)
            {
                color = red << 16;
            }
        }
    }
    if(argsLength > 1 && args[1]->IsInt32())
    {
        auto maybeLocalGreen = args[1]->ToInt32(args.GetIsolate()->GetCurrentContext());
        if(!maybeLocalGreen.IsEmpty())
        {
            auto green = maybeLocalGreen.ToLocalChecked()->Value();
            if(green>=0 && green <= 255)
            {
                color |= green << 8;
            }
        }
    }
    if(argsLength > 2 && args[2]->IsInt32())
    {
        auto maybeLocalBlue = args[2]->ToInt32(args.GetIsolate()->GetCurrentContext());
        if(!maybeLocalBlue.IsEmpty())
        {
            auto blue = maybeLocalBlue.ToLocalChecked()->Value();
            if(blue>=0 && blue <= 255)
            {
                color |= blue;
            }
        }
    }
    jsKeyboard->keyboard->SetAllKeys(color);
    jsKeyboard->keyboard->Commit();
}
void JsKeyboard::ToString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::HandleScope handle_scope(args.GetIsolate());
    v8::Local<v8::Object> self = args.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));    
    void* ptr = wrap->Value();
    JsKeyboard* jsKeyboard = static_cast<JsKeyboard*>(ptr);    
    args.GetReturnValue().Set(v8::String::NewFromUtf8(args.GetIsolate(), 
                                                      Utils::ToStr(jsKeyboard->keyboard->ToString()).c_str(),
                                                      v8::NewStringType::kNormal).ToLocalChecked());            
    
}
