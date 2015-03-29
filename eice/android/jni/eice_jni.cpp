
#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <android/log.h> // LOGD, LOGE ...

//#include <JNIHelp.h>
//#include <android_runtime/AndroidRuntime.h>
//#include "plw.h"

//#include "MyUtilAndroidJni.h"

//#include "SimpleAppLog.h"

#include "eice.h"


#define LOGTAG    "eice-jni"

#define dbgi(...) __android_log_print(ANDROID_LOG_INFO, LOGTAG,  __VA_ARGS__)
#define dbgw(...) __android_log_print(ANDROID_LOG_WARN, LOGTAG,  __VA_ARGS__)
#define dbge(...) __android_log_print(ANDROID_LOG_ERROR, LOGTAG,  __VA_ARGS__)
#define PRNRAW(...)  dbgi(__VA_ARGS__)

#define eice_dbgi(...)  __android_log_print(ANDROID_LOG_INFO, "ice-core",  __VA_ARGS__)


#if 0
#define dbgvv(...) dbgi(__VA_ARGS__)
#else
#define dbgvv(...)
#endif

#define MY_ANDROID_API  __attribute__ ((visibility ("default")))
#define MY_ANDROID_JNI_API
#define MY_ANDROID_JNI_ONLOAD_API MY_ANDROID_API

#define JNI_CLASS_NAME "com/easemob/media/EIce"

int eice_init();
int eice_new_caller(const char* config, char * local_content, int * p_local_content_len,
                    eice_t * pobj);
struct jni_context_st
{
    eice_t eice;
    char local_content[8*1024];
    int local_content_len;
    char nego_result[8*1024];
    int nego_result_len;
};

typedef struct jni_context_st * jni_context_t;

static int gEIceInited = 0;


static void _eice_log_func(int level, const char * data, int len){
    eice_dbgi("%s", data);
}

static void initJniContext(jni_context_t ctx){
    ctx->eice = 0;
    ctx->local_content_len = 0;
    ctx->nego_result_len = 0;
}

static jni_context_t createContext(){
    jni_context_t ctx = (jni_context_t) malloc(sizeof(struct jni_context_st));
    memset(ctx, 0, sizeof(sizeof(struct jni_context_st)));
    initJniContext(ctx);
    dbgi("createContext: ctx = %p", ctx);
    return ctx;
}

static void deleteContext(jni_context_t ctx){
    if(!ctx) {
        dbgw("deleteContext: warning, ctx is NULL");
        return;
    }
    
    dbgi("deleteContext: ctx=%p, eice=%p", ctx, ctx->eice);
    
    if(ctx->eice){
        dbgi("deleteContext: free eice %p", ctx->eice);
        eice_free(ctx->eice);
        ctx->eice = 0;
    }else{
        dbgw("deleteContext: warning, eice is NULL");
    }
    free(ctx);
}

static void checkInit()
{
    if(gEIceInited) return;
    eice_init();
    eice_set_log_func(_eice_log_func);
    gEIceInited = 1;
    dbgi("check init OK");
}

static void EIceJni_Init(JNIEnv * env, jobject thiz){
    checkInit();
}

static jlong EIceJni_NewCaller(JNIEnv * env, jobject thiz, jstring jstrConfig){
    jni_context_t ctx = 0;
    const char* config = 0;
    int ret = -1;
    do{
        ctx = createContext();
        config = (env)->GetStringUTFChars(jstrConfig, 0);
        ret =eice_new_caller(config, ctx->local_content, &ctx->local_content_len, &ctx->eice);
        if(ret !=0){
            dbge("eice_new_caller return fail, ret=%d!!!", ret);
            break;
        }
        ctx->local_content[ctx->local_content_len] = '\0';
        ret = 0;
        dbgi("EIceJni_NewCaller ok, ctx=%p, eice=%p", ctx, ctx->eice);
    }while(0);
    
//    dbgi("eice_new_caller ret=%d", ret);
    if(ret){
        if(ctx){
            dbgi("EIceJni_NewCaller: something wrong, delete ctx");
            deleteContext(ctx);
            ctx = 0;
        }
    }
    
    if(config) {
        (env)->ReleaseStringUTFChars(jstrConfig, config);
        config = 0;
    }
    
    return (jlong)ctx;
}

static
jlong EIceJni_NewCallee(JNIEnv * env, jobject thiz, jstring jstrConfig, jstring jstrRemoteContent)
{
    jni_context_t ctx = 0;
    const char* config = 0;
    const char* remoteContent = 0;
    int ret = -1;
    
    do{
        ctx = createContext();
        config = (env)->GetStringUTFChars(jstrConfig, 0);
        remoteContent = (env)->GetStringUTFChars(jstrRemoteContent, 0);
        jsize remoteContentLen = (env)->GetStringUTFLength(jstrRemoteContent);
        ret =eice_new_callee(config, remoteContent, remoteContentLen, ctx->local_content, &ctx->local_content_len, &ctx->eice);
        if(ret !=0){
            dbge("eice_new_callee return fail, ret=%d!!!", ret);
            break;
        }
        ctx->local_content[ctx->local_content_len] = '\0';
        ret = 0;
        dbgi("EIceJni_NewCallee ok, ctx=%p, eice=%p", ctx, ctx->eice);
    }while(0);
    
    if(ret){
        if(ctx){
            dbgi("EIceJni_NewCallee: something wrong, delete ctx");
            deleteContext(ctx);
            ctx = 0;
        }
    }
    
    if(config) {
        (env)->ReleaseStringUTFChars(jstrConfig, config);
        config = 0;
    }
    if(remoteContent) {
        (env)->ReleaseStringUTFChars(jstrRemoteContent, remoteContent);
        remoteContent = 0;
    }
    
    return (jlong)ctx;
}

static jstring EIceJni_GetLocalContent(JNIEnv * env, jobject thiz, jlong jctx){
    jni_context_t ctx = (jni_context_t)jctx;
    jstring jstrLocalContent = (env)->NewStringUTF(ctx->local_content);
    return jstrLocalContent;
}

static
void EIceJni_CallerNego(JNIEnv * env, jobject thiz, jlong jctx, jstring jstrRemoteContent)
{
    jni_context_t ctx = (jni_context_t)jctx;
    const char* remoteContent = 0;
    int ret = -1;
    do{
        remoteContent = (env)->GetStringUTFChars(jstrRemoteContent, 0);
        jsize remoteContentLen = (env)->GetStringUTFLength(jstrRemoteContent);
        ret = eice_caller_nego(ctx->eice, remoteContent, remoteContentLen, 0, 0);
        if(ret !=0){
            dbge("eice_new_callee return fail, ret=%d!!!", ret);
            break;
        }
    }while(0);

    if(remoteContent) {
        (env)->ReleaseStringUTFChars(jstrRemoteContent, remoteContent);
        remoteContent = 0;
    }
}

static
void EIceJni_FreeCall(JNIEnv * env, jobject thiz, jlong jctx)
{
    jni_context_t ctx = (jni_context_t)jctx;
    
    if(!ctx){
        dbge("EIceJni_FreeCall: ctx is NULL !!!");
        return;
    }
    
    deleteContext(ctx);
}

static
jstring EIceJni_GetNegoResult(JNIEnv * env, jobject thiz, jlong jctx){
    jni_context_t ctx = (jni_context_t)jctx;
    if(!ctx){
        dbge("EIceJni_GetNegoResult: ctx is NULL !!!");
        return 0;
    }
    
    if(!ctx->eice){
        dbge("EIceJni_GetNegoResult: eice is NULL !!!");
        return 0;
    }
    
    int ret = eice_get_nego_result(ctx->eice, ctx->nego_result, &ctx->nego_result_len);
    if(ret){
        //dbge("eice_get_nego_result: return %d !!!", ret);
        return 0;
    }
    ctx->nego_result[ctx->nego_result_len] = 0;
    
    jstring jstrNegoResult = (env)->NewStringUTF(ctx->nego_result);
    return jstrNegoResult;
}


static JNINativeMethod gNativeMethods[] =
{
    {"nativeInitEIce",         "()V",			(void*) EIceJni_Init},
    {"nativeNewCaller",         "(Ljava/lang/String;)J",			(void*) EIceJni_NewCaller},
    {"nativeNewCallee",         "(Ljava/lang/String;Ljava/lang/String;)J",			(void*) EIceJni_NewCallee},
    {"nativeGetLocalContent",   "(J)Ljava/lang/String;",			(void*) EIceJni_GetLocalContent},
    
    {"nativeCallerNego",        "(JLjava/lang/String;)V",			(void*) EIceJni_CallerNego},
    {"nativeFreeCall",          "(J)V",                          (void*) EIceJni_FreeCall},
    {"nativeGetNegoResult",     "(J)Ljava/lang/String;",			(void*) EIceJni_GetNegoResult},
    
};

//static int gNativeMethodsNum = NELEM(gNativeMethods);
static int gNativeMethodsNum = sizeof(gNativeMethods) / sizeof(gNativeMethods[0]);

int register_eice_jni(JavaVM * vm){
    JNIEnv *env = NULL;
    jint result = 0;
    
    
    
    PRNRAW("\n\n\n\n\n\n\n");
    PRNRAW("register_eice_jni() +++++++++++++++++++  \n\n");
    
    //ptrace(PTRACE_TRACEME,0 ,0 ,0);
    
    //    plw_init();
    //    plw_set_print_func(PrintLogMsg);
    
    do
    {
        if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK)
        {
            result = -1;
            PRNRAW("ERROR: GetEnv failed\n");
            break;
        }
        
        /*
         JNINativeMethod gJNIMethods[] =
         {
         {JNI_NATIVEINIT_STR,				JNI_NATIVEINIT_PARAM,			(void*) JniHttpSCInit},
         {JNI_NATIVEEXIT_STR,				JNI_NATIVEEXIT_PARAM,            		(void*) JniHttpSCExit},
         };*/
        
        //dbgi("JNI_NATIVEINIT_STR=%s\n", JNI_NATIVEINIT_PARAM);
        //dbgi("JNI_NATIVEINIT_PARAM=%s\n", JNI_NATIVEINIT_PARAM);
        //dbgi("JNI_NATIVEEXIT_STR=%s\n", JNI_NATIVEEXIT_STR);
        //dbgi("JNI_NATIVEEXIT_PARAM=%s\n", JNI_NATIVEEXIT_PARAM);
        
        
        PRNRAW("register native ...\n");
        
        //        result = android::AndroidRuntime::registerNativeMethods(env, JNI_CLASS_NAME, gNativeMethods, gNativeMethodsNum);
        
        jclass clazz;
        
        /* look up the class */
        //clazz = (*env)->FindClass(env, JNI_CLASS_NAME );
        clazz = (env)->FindClass(JNI_CLASS_NAME );
        if (clazz == NULL) {
            dbge("Can't find class %s\n", JNI_CLASS_NAME);
            return -1;
        }
        
        result = env->RegisterNatives(clazz, gNativeMethods, gNativeMethodsNum);
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

#ifndef EICE_STATIC
MY_ANDROID_JNI_ONLOAD_API jint JNI_OnLoad(JavaVM * vm, void *reserved)
{
    (void) reserved;
    return register_eice_jni(vm);
}
#endif

