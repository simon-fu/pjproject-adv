#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <android/log.h>
#include <string>
#include <stdio.h>

#ifdef __cplusplus
extern "C" int register_eice_jni(JavaVM * vm);
extern "C" int register_eice_other_jni(JavaVM * vm);
#endif

#if 0
struct rtp;
void SendAudio(struct rtp *client,unsigned char *buffer,int data_len,u_int64_t pts);
extern "C" int rtp_recv(struct rtp *session, struct timeval *timeout, uint32_t curr_rtp_ts);
void test_rtp(int a){
    //    SendAudio(0, 0, 0, 0);
    rtp_recv(0,0,0);
}
#else
#define test_rtp(x)
#endif

#define MY_ANDROID_API  __attribute__ ((visibility ("default")))
#define MY_ANDROID_JNI_API
#define MY_ANDROID_JNI_ONLOAD_API MY_ANDROID_API

MY_ANDROID_JNI_ONLOAD_API jint JNI_OnLoad(JavaVM * vm, void *reserved)
{
    //    (void) reserved;
    
    int ret = 0;
    
//    ret = register_eice_other_jni(vm);
//    if(ret != 0) return ret;
    test_rtp(0);

    
    ret = register_eice_jni(vm);
    if(ret != 0) return ret;
    
    std::string hello("hello");
    printf("%s", hello.c_str());
    return 0;
}

