#include "utils.h"

#include <iostream>
#include <fstream>


std::string Utils::mainScriptFolder = "";

v8::Local<v8::Value> Utils::RunScript(const v8::Local<v8::String>& source, const v8::Local<v8::Context>& context)
{
    v8::TryCatch tryCatch;
    v8::MaybeLocal<v8::Script> script = v8::Script::Compile(context, source);//.ToLocalChecked();
    if(script.IsEmpty())
    {
        if(tryCatch.HasCaught())
        {
            ReportException(&tryCatch,context);
        }
    }
    else 
    {
        // Run the script to get the result.
        v8::TryCatch tryCatch;
        v8::MaybeLocal<v8::Value> result = script.ToLocalChecked()->Run(context);
        if(!result.IsEmpty())
        {
            return result.ToLocalChecked();
        }
        else
        {
            if(tryCatch.HasCaught())
            {
                ReportException(&tryCatch,context);
            }
        }
    }    
    return v8::Undefined(context->GetIsolate());
}

const char* Utils::ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

void Utils::ReportException(v8::TryCatch* try_catch, const v8::Local<v8::Context>& context) 
{
  v8::HandleScope handle_scope(context->GetIsolate());
  v8::String::Utf8Value exception(try_catch->Exception());
  const char* exception_string = ToCString(exception);
  v8::Handle<v8::Message> message = try_catch->Message();
  if (message.IsEmpty()) {
    // V8 didn't provide any extra information about this error; just
    // print the exception.
    std::cout << exception_string << std::endl;
  } else {
    // Print (filename):(line number): (message).
    v8::String::Utf8Value filename(message->GetScriptResourceName());
    const char* filename_string = ToCString(filename);
    int linenum = message->GetLineNumber();
    std::cout << filename_string << ":" << linenum << ": "<< exception_string << std::endl;
    // Print line of source code.
    v8::String::Utf8Value sourceline(message->GetSourceLine());
    const char* sourceline_string = ToCString(sourceline);
    std::cout << sourceline_string << std::endl;
    // Print wavy underline (GetUnderline is deprecated).
    v8::Maybe<int> start = message->GetStartColumn(context);
    if(start.IsJust()) 
    {
        for (int i = 0; i < start.ToChecked(); i++) 
        {
            std::cout<<" ";
        }
    }
    v8::Maybe<int> end = message->GetEndColumn(context);
    if(start.IsJust() && end.IsJust())
    {
        for (int i = start.ToChecked(); i < end.ToChecked(); i++) 
        {
            std::cout<<"^";
        }
    }
    std::cout<<std::endl;
    v8::String::Utf8Value stack_trace(try_catch->StackTrace());
    if (stack_trace.length() > 0) 
    {
      const char* stack_trace_string = ToCString(stack_trace);
      std::cout<< stack_trace_string << std::endl;
    }
  }
}

std::string Utils::LoadScript(const std::string& fileName, const std::string& base_folder) 
{
    std::string script;
    if(base_folder.empty()) 
    {
        script = fileName;
    } else 
    {
        script = base_folder +'/' + fileName;
    }
    std::ifstream file(script.c_str());
    std::string result;
    char buf[4096];
    while(file.good() && !file.eof())
    {
        auto read = file.readsome(buf,4096);
        if(read <= 0) break;
        result.append(buf,read);
    }
    
    return result;
}
