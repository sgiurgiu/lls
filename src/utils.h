#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <v8.h>

class Utils
{
private:
    Utils(){}
public:    
    static v8::Local<v8::Value> RunScript(const v8::Local<v8::String>& source, const v8::Local<v8::Context>& context);
    static std::string LoadScript(const std::string& fileName, const std::string& base_folder = "");
    static void ReportException(v8::TryCatch* try_catch, const v8::Local<v8::Context>& context);
    static const char* ToCString(const v8::String::Utf8Value& value);
    static std::string ToStr(const std::wstring& wstr);
    static void Sleep(int32_t milliseconds);
    static std::string mainScriptFolder;
};

#endif // UTILS_H
