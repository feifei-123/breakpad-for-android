#include <stdio.h>
#include <jni.h>
#include <android/log.h>
#include <semaphore.h>

#include "client/linux/handler/exception_handler.h"
#include "client/linux/handler/minidump_descriptor.h"

#include "com_sogou_translate_jni_BreakpadInit.h"

#define LOG_TAG "feifei_native"

#include "pthread.h"


#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


//global对象 在on_unload方法里 需要手动释放
jobject global_infohelper;
//jclass global_class = NULL;


JavaVM* struct_jvm = NULL;
jmethodID struct_method;

pthread_t pthread;//线程对象
//信号量
sem_t bin_sem;
void tryDump();




extern void *threadDoThings(void *data);
bool DumpCallback(const google_breakpad::MinidumpDescriptor &descriptor,
                  void *context,
                  bool succeeded) {
    ALOGD("===============feifei crash happened ================");
    ALOGD("Dump path: %s ,%d\n", descriptor.path(),succeeded);
    sem_post(&bin_sem);
    sleep(2);
    return succeeded;
}


void *threadDoThings(void *data)
{
    ALOGD("feifei  jni thread do things, wait signal");
    sem_wait(&bin_sem);
    ALOGD("feifei  receive singal, do tryDump");
    tryDump();
    ALOGD("feifei  after tryDump");
    pthread_exit(&pthread);
}
void initThread(){

    int res = sem_init(&bin_sem, 0, 0);   /* 初始化信号量，并且设置初始值为0*/
    if (res != 0) {
        ALOGD("Semaphore initialization failed");
        exit(EXIT_FAILURE);
    }
    pthread_create(&pthread, NULL, threadDoThings, NULL);

}

extern "C"
JNIEXPORT void JNICALL Java_com_sogou_translate_jni_BreakpadInit_go2crash
        (JNIEnv *env, jclass clazz){
    ALOGD("===============create a native crash ================");
        char *ptr = NULL; *ptr = 1;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_sogou_translate_jni_BreakpadInit_initBreakpadNative(JNIEnv *env, jclass type, jstring path_,
                                                         jobject infoHelper) {

    //初始化BreadPad 指定minidump文件保存的位置
    const char *path = env->GetStringUTFChars(path_, 0);
    google_breakpad::MinidumpDescriptor descriptor(path);
    static google_breakpad::ExceptionHandler eh(descriptor, NULL, DumpCallback, NULL, true, -1);
    env->ReleaseStringUTFChars(path_, path);

    ALOGD("===============feifei initBreakpadNative ================");

    global_infohelper = env->NewGlobalRef(infoHelper);

    initThread();

}


/**
 * JNI 加载之初
 * @param vm
 * @param reserved
 * @return
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {

    ALOGD("===============feife JNI_OnLoad ================");

    if (vm == NULL)
    {
        return JNI_ERR;
    }

    JNIEnv *env = NULL;
    struct_jvm = vm;

    if(vm->GetEnv((void**)&env,JNI_VERSION_1_6)!=JNI_OK){
        return JNI_ERR;
    }

//    jclass  myClass = (env)->FindClass("com/online/breakpad/InfoHelper");
//    global_class = (jclass)env->NewGlobalRef(myClass);

//    struct_method = (env)->GetMethodID(global_class,"dumpLogcat","()V");

    return JNI_VERSION_1_6;
}

/**
 * 当虚拟机释放该C库时，则会调用JNI_OnUnload()函数来进行善后清除动作
 * @param vm
 * @param reserved
 */
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
    ALOGD("===============feife JNI_OnUnload ================");
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return;
    }
//    env->DeleteGlobalRef(global_class);
    env->DeleteGlobalRef(global_infohelper);
    sem_destroy(&bin_sem);
    return;

}



void tryDump(){
    JNIEnv *env = NULL;
    int getEnvStat;
    bool isAttached = false;
    getEnvStat = struct_jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if(env == NULL){
        ALOGD("feifei status was %d ,env was null,%p",getEnvStat,env);
    } else{
        ALOGD("feifei status was %d ,env was not null %p",getEnvStat,env);
    }


    if (getEnvStat == JNI_EDETACHED||getEnvStat == JNI_ERR||getEnvStat == JNI_EVERSION) {
        if (struct_jvm->AttachCurrentThread(&env, NULL))////将当前线程注册到虚拟机中
        {
            return;
        }
        isAttached = true;
    }
    ALOGD("feifei isAttached %d,env address:%p",isAttached,env);
    //实例化该类
//    jobject jobject = env->AllocObject(global_class);//分配新 Java 对象而不调用该对象的任何构造函数。返回该对象的引用。

    jobject jobject = global_infohelper;
    if((env)->IsSameObject(jobject,NULL)){
        ALOGD("feifei AllocObject is null ");
    } else{
        ALOGD("feifei AllocObject not null,%p",jobject);
    }


    if(struct_jvm == NULL){
        ALOGD("feifei struct_jvm is null ");
    } else{
        ALOGD("feifei struct_jvm is not null %p",struct_jvm);
    }

    jclass  myClass = (env)->GetObjectClass(jobject);

    struct_method = (env)->GetMethodID(myClass,"dumpLogcat","()V");
    if(struct_method == NULL){
        ALOGD("feifei struct_method is null ");
    } else{
        ALOGD("feifei struct_method is not null %p",struct_method);
    }

    //调用Java方法
    (env)->CallVoidMethod(jobject, struct_method);

    ALOGD("feifei CallVoidMethod");

    if (isAttached) {
        struct_jvm->DetachCurrentThread();
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_online_breakpad_BreakpadInit_testInfoHelper(JNIEnv *env, jclass type, jobject infoHelper) {

    jclass  clazz = env->GetObjectClass(infoHelper);
    if(clazz == NULL){
        ALOGD("feifei clazz was null");
    } else{
        ALOGD("feifei clazz was not  null");
    }
    jmethodID jumpLocat =  env->GetMethodID(clazz, "dumpLogcat", "()V");
    if(jumpLocat == NULL) {
        ALOGD("feifei jumpLocat was null");
    } else{
        ALOGD("feifei jumpLocat was not null");
    }
    env->CallVoidMethod(infoHelper,jumpLocat);

    ALOGD("feifei jumpLocat was called");

}




