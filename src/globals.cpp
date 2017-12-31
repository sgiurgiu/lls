#include "globals.h"
#include "utils.h"
#include <iostream>
#include <iomanip>
#include <ctime>

void Globals::Version(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
    args.GetReturnValue().Set(
      v8::String::NewFromUtf8(args.GetIsolate(), "0.1",
                              v8::NewStringType::kNormal).ToLocalChecked());
}

void Globals::Log(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);    
    auto argsLength = args.Length();
    std::cout << "[" << std::put_time(&tm, "%c %Z") << "] ";
    for(int i=0;i<argsLength-1;i++)
    {
        std::cout << Utils::ToCString(v8::String::Utf8Value(args[i])) << " ";
    }
    if(argsLength > 0)
    {
        std::cout << Utils::ToCString(v8::String::Utf8Value(args[argsLength-1]));
    }
    std::cout << std::endl;
    
  /*args.GetReturnValue().Set(
      v8::String::NewFromUtf8(args.GetIsolate(), v8::V8::GetVersion(),
                              v8::NewStringType::kNormal).ToLocalChecked());*/
}
void Globals::GetNum(v8::Local<v8::String> property,const v8::PropertyCallbackInfo<v8::Value>& info)
{
    std::cout << Utils::ToCString(v8::String::Utf8Value(property)) << std::endl;
    info.GetReturnValue().Set(v8::Number::New(info.GetIsolate(), 1.7));
}
void Globals::Sleep(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t milliseconds = 1000;
    if(args.Length() > 0 && args[0]->IsInt32())
    {
        auto maybeAmount = args[0]->ToInt32(args.GetIsolate()->GetCurrentContext());
        if(!maybeAmount.IsEmpty())
        {
            milliseconds = maybeAmount.ToLocalChecked()->Value();
            milliseconds = milliseconds > 0 ? milliseconds : 1000;
        }
    }
    Utils::Sleep(milliseconds);
}

void Globals::Require(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
    auto argsLength = args.Length();
    for(int i=0;i<argsLength;i++)
    {
        v8::MaybeLocal<v8::String> fileName = args[i]->ToString(args.GetIsolate()->GetCurrentContext());
        if(!fileName.IsEmpty())
        {
            std::string scriptSource = Utils::LoadScript(
                        Utils::ToCString(v8::String::Utf8Value(fileName.ToLocalChecked())),
                        Utils::mainScriptFolder);
            v8::Local<v8::String> source = v8::String::NewFromUtf8(args.GetIsolate(), 
                                                                   scriptSource.c_str(),
                                                                   v8::NewStringType::kNormal).ToLocalChecked();            
            auto result = Utils::RunScript(source,args.GetIsolate()->GetCurrentContext());
            args.GetReturnValue().Set(result);
        }
    }    
}
