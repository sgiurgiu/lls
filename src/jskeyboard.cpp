#include "jskeyboard.h"
#include "utils.h"

JsKeyboard::JsKeyboard(v8::Isolate* isolate, const Keyboard * const keyboard): isolate(isolate),keyboard(keyboard)
{
}
void JsKeyboard::GetDefaultKeyboard(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::HandleScope handle_scope(args.GetIsolate());
    auto keyboards = Keyboard::GetKeyboards();    
    if(!keyboards->empty())
    {
        v8::Local<v8::Object> jskeyboard = WrapKeyboard(args.GetIsolate(),&(*keyboards->begin()));
        args.GetReturnValue().Set(jskeyboard);
    }
    else
    {
        args.GetReturnValue().Set(v8::Null(args.GetIsolate()));
    }    
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
    keyboardTemplate->Set(v8::String::NewFromUtf8(isolate,"SetAllKeysColor",v8::NewStringType::kNormal)
                        .ToLocalChecked(), v8::FunctionTemplate::New(isolate, SetAllKeys));
    keyboardTemplate->Set(v8::String::NewFromUtf8(isolate,"UpdateKeys",v8::NewStringType::kNormal)
                        .ToLocalChecked(), v8::FunctionTemplate::New(isolate, UpdateKeys));
    keyboardTemplate->Set(v8::String::NewFromUtf8(isolate,"UpdateKey",v8::NewStringType::kNormal)
                        .ToLocalChecked(), v8::FunctionTemplate::New(isolate, UpdateKeys));
    keyboardTemplate->Set(v8::String::NewFromUtf8(isolate,"GetKeys",v8::NewStringType::kNormal)
                        .ToLocalChecked(), v8::FunctionTemplate::New(isolate, GetAllKeys));
    keyboardTemplate->Set(v8::String::NewFromUtf8(isolate,"GetKeysMatrix",v8::NewStringType::kNormal)
                        .ToLocalChecked(), v8::FunctionTemplate::New(isolate, GetKeysMatrix));
    v8::Local<v8::Object> obj = keyboardTemplate->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
    obj->SetInternalField(0, v8::External::New(isolate, jsKeyboard));
    return handle_scope.Escape(obj);
}

void JsKeyboard::UpdateKeys(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::HandleScope handle_scope(args.GetIsolate());
    JsKeyboard* jsKeyboard = nullptr;
    {
        v8::Local<v8::Object> self = args.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void* ptr = wrap->Value();
        jsKeyboard = static_cast<JsKeyboard*>(ptr);
    }
    int argsLength = args.Length();
    if(argsLength > 0 && (args[0]->IsArray() || args[0]->IsObject()))
    {
        bool colorSpecified = false;
        uint32_t color = 0;
        if(argsLength == 2 && args[1]->IsUint32())
        {
            auto maybeLocalColor = args[1]->ToUint32(args.GetIsolate()->GetCurrentContext());
            if(!maybeLocalColor.IsEmpty())
            {
                color = maybeLocalColor.ToLocalChecked()->Value();
                colorSpecified = true;
            }
        } 
        else if (argsLength == 4 && args[1]->IsUint32() && args[2]->IsUint32() && args[3]->IsUint32())
        {
            auto maybeLocalRed = args[1]->ToUint32(args.GetIsolate()->GetCurrentContext());
            auto maybeLocalGreen = args[2]->ToUint32(args.GetIsolate()->GetCurrentContext());
            auto maybeLocalBlue = args[3]->ToUint32(args.GetIsolate()->GetCurrentContext());
            if(!maybeLocalRed.IsEmpty() && !maybeLocalGreen.IsEmpty() && !maybeLocalBlue.IsEmpty())
            {
                auto red   = maybeLocalRed.ToLocalChecked()->Value();
                auto green = maybeLocalGreen.ToLocalChecked()->Value();
                auto blue  = maybeLocalBlue.ToLocalChecked()->Value();
                color = ((red & 0xff) << 16) | ((green & 0xff) << 8) | (blue & 0xff);
                colorSpecified = true;
            }
        }

        Keys keys;
        if(args[0]->IsArray())//UpdateKeys
        {
            v8::Array* keysArray = v8::Array::Cast(*args[0]);
            keys.resize(keysArray->Length());
            for(uint32_t i=0;i < keysArray->Length();i++)
            {
                auto maybeLocalKey = keysArray->Get(args.GetIsolate()->GetCurrentContext(),i);
                if(!maybeLocalKey.IsEmpty())
                {
                    auto maybeLocalObject = maybeLocalKey.ToLocalChecked()->ToObject(args.GetIsolate()->GetCurrentContext());
                    if(!maybeLocalObject.IsEmpty())
                    {
                        auto keyExternalWrap = v8::Local<v8::External>::Cast(maybeLocalObject.ToLocalChecked()->GetInternalField(0));
                        void* ptr = keyExternalWrap->Value();
                        Key* key = static_cast<Key*>(ptr);
                        if(colorSpecified) key->color = color;
                        keys[i] = (*key);
                    }
                }
            }
        }
        else if(args[0]->IsObject())//UpdateKey
        {
            auto maybeLocalObject = args[0]->ToObject(args.GetIsolate()->GetCurrentContext());
            if(!maybeLocalObject.IsEmpty())
            {
                auto keyExternalWrap = v8::Local<v8::External>::Cast(maybeLocalObject.ToLocalChecked()->GetInternalField(0));
                void* ptr = keyExternalWrap->Value();
                Key* key = static_cast<Key*>(ptr);
                if(colorSpecified) key->color = color;
                keys.push_back(*key);
            }
        }
        jsKeyboard->keyboard->SetKeys(keys);
        jsKeyboard->keyboard->Commit();
    }
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

void JsKeyboard::GetKeysMatrix(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::HandleScope handle_scope(args.GetIsolate());
    v8::Local<v8::Object> self = args.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));    
    void* ptr = wrap->Value();
    JsKeyboard* jsKeyboard = static_cast<JsKeyboard*>(ptr);
    auto keysMatrix = jsKeyboard->keyboard->GetKeysMatrix();
    auto rows = keysMatrix.size();
    auto keysMatrixArray = v8::Array::New(args.GetIsolate(),rows);
    size_t i = 0;
    for(const auto& row : keysMatrix)
    {
        size_t j = 0;
        auto rowArray = v8::Array::New(args.GetIsolate(),row.size());
        for(const auto& key : row)
        {
            v8::Local<v8::Value> jsKey = v8::Null(args.GetIsolate());
            if(key.code != KeyCode::none)
            {
                jsKey = WrapKey(args.GetIsolate(),key);
            }
            auto result = rowArray->Set(args.GetIsolate()->GetCurrentContext(), j, jsKey);
            if(result.IsNothing())
            {
            }
            j++;
        }
        auto result = keysMatrixArray->Set(args.GetIsolate()->GetCurrentContext(), i, rowArray);
        if(result.IsNothing())
        {
        }
        i++;
    }
    args.GetReturnValue().Set(keysMatrixArray);
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
    if(argsLength > 0 && args[0]->IsUint32())
    {
        auto maybeLocalRed = args[0]->ToUint32(args.GetIsolate()->GetCurrentContext());
        if(!maybeLocalRed.IsEmpty())
        {
            auto red = maybeLocalRed.ToLocalChecked()->Value();
            if(argsLength == 1)
            {
                //if we only have 1 argument, interpret that as the color
                //e.g. 0xffaa11
                color = red;
            }
            else
            {
                color = (red & 0xff) << 16;
            }
        }
    }
    if(argsLength > 1 && args[1]->IsUint32())
    {
        auto maybeLocalGreen = args[1]->ToUint32(args.GetIsolate()->GetCurrentContext());
        if(!maybeLocalGreen.IsEmpty())
        {
            auto green = maybeLocalGreen.ToLocalChecked()->Value();
            color |= (green & 0xff) << 8;
        }
    }
    if(argsLength > 2 && args[2]->IsUint32())
    {
        auto maybeLocalBlue = args[2]->ToUint32(args.GetIsolate()->GetCurrentContext());
        if(!maybeLocalBlue.IsEmpty())
        {
            auto blue = maybeLocalBlue.ToLocalChecked()->Value();
            color |= (blue & 0xff);
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
