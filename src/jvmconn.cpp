#include "jvmconn.h"
#include "boost/filesystem.hpp"

bool JVMConn::GetJniEnv(JavaVM *vm_, JNIEnv **env_) {
    bool did_attach_thread = false;
    *env_ = nullptr;
    // Check if the current thread is attached to the VM
    auto get_env_result = vm_->GetEnv((void **) env_, JNI_VERSION_1_8);
    if (get_env_result == JNI_EDETACHED) {
        if (vm_->AttachCurrentThread(reinterpret_cast<void **>(env_), nullptr) == JNI_OK) {
            did_attach_thread = true;
        } else {
            // Failed to attach thread. Throw an exception if you want to.
        }
    } else if (get_env_result == JNI_EVERSION) {
        // Unsupported JNI version. Throw an exception if you want to.
    }
    return did_attach_thread;
}

JVMConn::JVMConn(Module *module) : module(module) {

    boost::filesystem::path class_path(module->config->class_path), connector_jar_path(
            module->config->connector_jar_path),
            lang3_jar_path(module->config->lang3_jar_path);

    std::string option_str = std::string("-Djava.class.path=") + boost::filesystem::canonical(class_path).string() + ":"
                             + boost::filesystem::canonical(connector_jar_path).string() + ":" +
                             boost::filesystem::canonical(lang3_jar_path).string();
    char *option_cstr = new char[option_str.size() + 1];
    memset(option_cstr, 0, option_str.size() + 1);
    memcpy(option_cstr, option_str.c_str(), option_str.size());
    options[0].optionString = option_cstr;

    vm_args.version = JNI_VERSION_1_8;
    vm_args.nOptions = 1;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = JNI_TRUE;
    int ret = JNI_CreateJavaVM(&vm, (void **) &env, &vm_args);
    if (ret < 0) std::cerr << "Unable to create JVM" << std::endl;
}

JVMConn::~JVMConn() {
    vm->DestroyJavaVM();
}

std::string JVMConn::callMethod(const char *className, const char *methodName, std::string &arg) {
    JNIEnv *my_env;
    jstring jstr = env->NewStringUTF(arg.c_str());

    jclass clazz = env->FindClass(className);
    std::string ret;
    std::cerr << "Calling " << className << " " << methodName << " " << arg << std::endl;
    jmethodID jmethodId = env->GetStaticMethodID(clazz, methodName,
                                                 "(Ljava/lang/String;)Ljava/lang/String;");
    if (jmethodId == nullptr) {
        std::cerr << "ERROR NO METHOD!" << std::endl;
    }

    jobject result = env->CallObjectMethod(clazz, jmethodId, jstr);
    if (env->ExceptionCheck()) env->ExceptionDescribe();

    const char *str = env->GetStringUTFChars((jstring) result, nullptr);

    ret.assign(str);
    env->ReleaseStringUTFChars((jstring) result, str);


    return ret;
}
