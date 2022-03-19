#pragma once

extern "C" {
#include "jni.h"
}

#include <string>
#include "module.h"

class JVMConn {
private:
    Module *module;
    JavaVM *vm{};
    JNIEnv *env{};
    JavaVMInitArgs vm_args{};
    JavaVMOption options[2]{};
public:
    bool GetJniEnv(JavaVM *vm_, JNIEnv **env_);

    explicit JVMConn(Module *module);

    ~JVMConn();

    std::string fetchAllLibs();

    std::string callMethod(const char *className, const char *methodName, std::string &arg, const char *methodSign,
                           bool haveResult);
};