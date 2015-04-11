
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <android/log.h>

#define JNI_CLASS_NAME "com/easemob/media/EIceOther"
#define LOGTAG    "eice-other"

#define dbgi(...) __android_log_print(ANDROID_LOG_INFO, LOGTAG,  __VA_ARGS__)
#define dbgw(...) __android_log_print(ANDROID_LOG_WARN, LOGTAG,  __VA_ARGS__)
#define dbge(...) __android_log_print(ANDROID_LOG_ERROR, LOGTAG,  __VA_ARGS__)
#define PRNRAW(...)  dbgi(__VA_ARGS__)

static jstring EIceOther_Hello(JNIEnv * env, jobject thiz, jstring jstrHello){
    char buf[512];
    const char * hello = (*env)->GetStringUTFChars(env, jstrHello, 0);
    sprintf(buf, "%s-echo from native", hello);
    jstring jstrEcho = (*env)->NewStringUTF(env, buf);
    (*env)->ReleaseStringUTFChars(env, jstrHello, hello);
    return jstrEcho;
}

static JNINativeMethod gNativeMethods[] =
{
    {"nativeHello",         "(Ljava/lang/String;)Ljava/lang/String;",			(void*) EIceOther_Hello},
};

//static int gNativeMethodsNum = NELEM(gNativeMethods);
static int gNativeMethodsNum = sizeof(gNativeMethods) / sizeof(gNativeMethods[0]);

int register_eice_other_jni(JavaVM * vm){
    JNIEnv *env = NULL;
    jint result = 0;
    
    PRNRAW("\n\n\n\n\n\n\n");
    PRNRAW("register_eice_other_jni() +++++++++++++++++++  \n\n");
    
    do
    {
        if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_4) != JNI_OK)
        {
            result = -1;
            PRNRAW("ERROR: GetEnv failed\n");
            break;
        }
        
        
        PRNRAW("register native ...\n");
        
        //        result = android::AndroidRuntime::registerNativeMethods(env, JNI_CLASS_NAME, gNativeMethods, gNativeMethodsNum);
        
        jclass clazz;
        
        /* look up the class */
        clazz = (*env)->FindClass(env, JNI_CLASS_NAME );
//        clazz = (env)->FindClass(JNI_CLASS_NAME );
        if (clazz == NULL) {
            dbge("Can't find class %s\n", JNI_CLASS_NAME);
            return -1;
        }
        
        result = (*env)->RegisterNatives(env, clazz, gNativeMethods, gNativeMethodsNum);
        if(result < 0)
        {
            //dbge("regeister jni class fail, error code %d \n", result);
            PRNRAW("regeister jni class fail, error code\n");
            break;
        }
        
        PRNRAW("register native OK\n");
        
        // success -- return valid version number
        result = JNI_VERSION_1_4;
    }while(0);
    
    return result;
}




