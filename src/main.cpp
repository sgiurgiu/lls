#include <iostream>
#include <string>


#include <libplatform/libplatform.h>
#include <v8.h>

#include "utils.h"
#include "globals.h"
#include "keyboard.h"
#include "jskeyboard.h"

void ExecuteMainScript(const std::string& scriptSource,v8::Isolate* isolate);
v8::Platform* SetupV8(const char*);
void DisposeV8(v8::Platform*);
void SetupIsolateAndExecute(const std::string& scriptSource);

int main(int argc, char **argv) {
    
    if(argc < 2) 
    {
        std::cerr << "Usage "<<argv[0]<<" script.js" <<std::endl;
        return 1;
    }
    
    if(!Keyboard::LoadAvailableKeyboards())
    {
        std::cerr<< "No supported keyboard was found" <<std::endl;
        return 1;
    }
    
    std::string mainScriptFile = std::string(argv[1]);
    Utils::mainScriptFolder = mainScriptFile.substr(0,mainScriptFile.find_last_of('/'));
    std::string scriptSource = Utils::LoadScript(mainScriptFile);
    
    v8::Platform* platform = SetupV8(argv[0]);
            
    SetupIsolateAndExecute(scriptSource);
    
    DisposeV8(platform);
    
    Keyboard::DisposeKeyboards();
    
    return 0;
}

void DisposeV8(v8::Platform* platform)
{
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete platform;    
}

v8::Platform* SetupV8(const char* path)
{
    v8::V8::InitializeICUDefaultLocation(path);
    v8::V8::InitializeExternalStartupData(path);
    v8::Platform* platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform);
    v8::V8::Initialize();
    return platform;
}

void SetupIsolateAndExecute(const std::string& scriptSource)
{
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate* isolate = v8::Isolate::New(create_params);
    
    ExecuteMainScript(scriptSource,isolate);
    
    // Dispose the isolate and tear down V8.
    isolate->Dispose();        
    delete create_params.array_buffer_allocator;    
}

void ExecuteMainScript(const std::string& scriptSource,v8::Isolate* isolate)
{    
    v8::Isolate::Scope isolate_scope(isolate);
    // Create a stack-allocated handle scope.
    v8::HandleScope handle_scope(isolate);
    
    v8::Local<v8::ObjectTemplate> globalTemplate = v8::ObjectTemplate::New(isolate);
    globalTemplate->Set(v8::String::NewFromUtf8(isolate,"version",v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, Globals::Version));
    globalTemplate->Set(v8::String::NewFromUtf8(isolate,"log",v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, Globals::Log));
    globalTemplate->Set(v8::String::NewFromUtf8(isolate,"require",v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, Globals::Require));
    globalTemplate->Set(v8::String::NewFromUtf8(isolate,"include",v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, Globals::Require));
    globalTemplate->Set(v8::String::NewFromUtf8(isolate,"sleep",v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, Globals::Sleep));
    globalTemplate->Set(v8::String::NewFromUtf8(isolate,"GetKeyboards",v8::NewStringType::kNormal).ToLocalChecked(), v8::FunctionTemplate::New(isolate, JsKeyboard::GetKeyboards));
    
    globalTemplate->SetAccessor(v8::String::NewFromUtf8(isolate,"num",v8::NewStringType::kNormal).ToLocalChecked(),Globals::GetNum);
    
    // Create a new context.
    v8::Local<v8::Context> context = v8::Context::New(isolate,NULL,globalTemplate);
    // Enter the context for compiling and running the hello world script.
    v8::Context::Scope context_scope(context);
    // Create a string containing the JavaScript source code.
    v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, scriptSource.c_str(),
                                                           v8::NewStringType::kNormal).ToLocalChecked();
    // Compile the source code and run it
    Utils::RunScript(source,context);
        
    //while(platform::PumpMessageLoop(platform,isolate,platform::MessageLoopBehavior::kDoNotWait));
}
