#ifndef __MyUtilAndroidJni_H__
#define __MyUtilAndroidJni_H__

//#pragma once
#include <jni.h>
#include "JNIHelp.h"
#include <android_runtime/AndroidRuntime.h>


#define JAVA_CLASS_VARIABLE(c, s)										\
    jclass class##c = (env)->FindClass(s);								\
    LOG_ASSERT(class##c, "Unable to find class "s);

#define JAVA_CLASS_OBJECT_FIELD(c, n, s)																		\
    jfieldID fieldID##c##n = (env)->GetFieldID(class##c, #n, s);												\
    LOG_ASSERT(fieldID##c##n, "Could not find field "#n" of "s);

#define JAVA_CLASS_INIT(c)																						\
	jmethodID init##c = env->GetMethodID(class##c, "<init>", "()V");											\
	LOG_ASSERT(init##c, "Could not init init"#c);

#define JAVA_CLASS_NEW_OBJECT(r, c)																				\
	jobject r = (env)->NewObject(class##c, init##c);															\
	LOG_ASSERT(r, "Could not create new object of "#c);

#define JAVA_CLASS_ALLOC_OBJECT(r, c)																			\
	jobject r = (env)->AllocObject(class##c);																	\
	LOG_ASSERT(r, "Could not alloc new object of "#c);

#define JAVA_CLASS_NEW_OBJECT_ARRAY(a, n, c)																	\
	jobjectArray a = (env)->NewObjectArray(n, class##c, NULL);													\
	LOG_ASSERT(a, "Could not create new "#n"object array of class "#c);

// to resolve ReferenceTable overflow (max=512) issue
#define JAVA_CLASS_DELETE_LOCAL_REFERENCE_TABLE(c)                                                                                                \
	 (env)->DeleteLocalRef(class##c);                                                                                                                                   


#define JAVA_CLASS_SET_OBJECT_FIELD(r, c, n, s)																	\
	(env)->SetObjectField(r, fieldID##c##n, s);																	\
	LOGV(#n" = %p\n", s);

#define JAVA_CLASS_SET_BOOLEAN_OBJECT_FIELD(r, c, n, s)															\
	(env)->SetBooleanField(r, fieldID##c##n, s);																\
	LOGV(#n" = %d\n", s);
#define JAVA_CLASS_SET_BYTE_OBJECT_FIELD(r, c, n, s)															\
	(env)->SetByteField(r, fieldID##c##n, s);																	\
	LOGV(#n" = %d\n", s);
#define JAVA_CLASS_SET_CHAR_OBJECT_FIELD(r, c, n, s)															\
	(env)->SetCharField(r, fieldID##c##n, s);																	\
	LOGV(#n" = %d\n", s);
#define JAVA_CLASS_SET_INTEGER_OBJECT_FIELD(r, c, n, s)															\
	(env)->SetIntField(r, fieldID##c##n, s);																	\
	LOGV(#n" = %d\n", s);
#define JAVA_CLASS_SET_STRING_OBJECT_FIELD(r, c, n, s)															\
	(env)->SetObjectField(r, fieldID##c##n, (env)->NewStringUTF(s));											\
	LOGV(#n" = %s\n", s);

#define JAVA_CLASS_SET_INT_ARRAY_REGION(a, s, l, b)															\
	(env)->SetIntArrayRegion(a,s,l,b);											                                                        


#define JAVA_CLASS_SET_OBJECT_FIELD_OF_VECTOR(p, i, r, c, n, o)													\
	if (p.size() > 0) {																							\
		JAVA_CLASS_SET_OBJECT_FIELD(r, c, n, o)																	\
	} else {																									\
		JAVA_CLASS_SET_OBJECT_FIELD(r, c, n, NULL)																\
	}

#define JAVA_CLASS_SET_OBJECT_FIELD_OF_VECTOR_WITH_FUNCTION(p, i, r, c, n, f)									\
	if (p.size() > 0) {																							\
		JAVA_CLASS_SET_OBJECT_FIELD(r, c, n, f(env, &(p[i])))													\
	} else {																									\
		JAVA_CLASS_SET_OBJECT_FIELD(r, c, n, NULL)																\
	}

#define JAVA_CLASS_SET_CHAR_OBJECT_FIELD_OF_VECTOR(p, i, r, c, n)												\
	if (p.size() > 0) {																							\
		JAVA_CLASS_SET_CHAR_OBJECT_FIELD(r, c, n, p[i].content)													\
	} else {																									\
		JAVA_CLASS_SET_CHAR_OBJECT_FIELD(r, c, n, 0)															\
	}
#define JAVA_CLASS_SET_STRING_OBJECT_FIELD_OF_VECTOR(p, i, r, c, n)												\
	if (p.size() > 0) {																							\
		JAVA_CLASS_SET_STRING_OBJECT_FIELD(r, c, n, p[i].content.c_str())										\
	} else {																									\
		JAVA_CLASS_SET_STRING_OBJECT_FIELD(r, c, n, "")															\
	}
#define JAVA_CLASS_SET_ATTRI_STRING_OBJECT_FIELD_OF_VECTOR(p, i, r, c, n, v)									\
	if (p.size() > 0) {																							\
		JAVA_CLASS_SET_STRING_OBJECT_FIELD(r, c, n, p[i].v.c_str())												\
	} else {																									\
		JAVA_CLASS_SET_STRING_OBJECT_FIELD(r, c, n, "")															\
	}

#define JAVA_CLASS_SET_INTEGER_OBJECT_FIELD_OF_VECTOR(p, i, r, c, n)											\
	if (p.size() > 0) {																							\
		JAVA_CLASS_SET_INTEGER_OBJECT_FIELD(r, c, n, p[i].content)												\
	} else {																									\
		JAVA_CLASS_SET_INTEGER_OBJECT_FIELD(r, c, n, 0)															\
	}

#define JAVA_CLASS_SET_BOOLEAN_OBJECT_FIELD_OF_VECTOR(p, i, r, c, n)											\
	if (p.size() > 0) {																							\
		JAVA_CLASS_SET_BOOLEAN_OBJECT_FIELD(r, c, n, p[i].content)												\
	} else {																									\
		JAVA_CLASS_SET_BOOLEAN_OBJECT_FIELD(r, c, n, 0)															\
	}



#define JAVA_CLASS_METHOD(m, c, mn, d)																						\
	jmethodID m = (env)->GetMethodID(class##c, mn, d);											\
	LOG_ASSERT(m, "Could not get "#c m d);

#define JAVA_CLASS_CALL_METHOD(o, m, ...)																	\
	(env)->CallObjectMethod(o, methodID##m, __VA_ARGS__);													\
	LOG_ASSERT(o, "Could not call method "#m"of obj "#o);


/*
  Z       boolean
  B       byte
  C       char
  S       short
  I       int
  J       long
  F       float
  D       double
  L fully-qualified-class;   fully-qualified-clas
*/


/*
V      void            void
Z       jboolean     boolean
I        jint              int
J       jlong            long
D      jdouble       double
F      jfloat            float
B      jbyte            byte
C      jchar           char
S      jshort          short


数组则以"["开始，用两个字符表示


[I       jintArray      int[]
[F     jfloatArray    float[]
[B     jbyteArray    byte[]
[C    jcharArray    char[]
[S    jshortArray   short[]
[D    jdoubleArray double[]
[J     jlongArray     long[]
[Z    jbooleanArray boolean[]


上面的都是基本类型。如果Java函数的参数是class，则以"L"开头，以";"结尾中间是用"/" 隔开的包及类名。而其对应的C函数名的参数则为jobject. 一个例外是String类，其对应的类为jstring


Ljava/lang/String; String jstring
Ljava/net/Socket; Socket jobject


如果JAVA函数位于一个嵌入类，则用$作为类名间的分隔符。

例如 "(Ljava/lang/String;Landroid/os/FileUtils$FileStatus;)Z"

*/


#define VOID_STRING_METHOD_ARGS "(Ljava/lang/String;)V"
#define VOID_BOOLEAN_METHOD_ARGS "(Z)V"
#define VOID_INTEGER_METHOD_ARGS "(I)V"

#define MAX_FUNCTIONS_NUMBER 16

struct JNI_Function
{
	int IsEqual(const char *methodName, const char *methodArgs);

	int SetMethod(JNIEnv *env, jclass clazz, const char *methodName, const char *methodArgs);

	int CallVoidMethod(JNIEnv *env, jclass clazz, jobject object, const char *methodName, const char *methodArgs, ...);

	int CallVoidStringMethod(JNIEnv *env, jclass clazz, jobject object, const char *methodName, const char *src);
	int CallVoidBooleanMethod(JNIEnv *env, jclass clazz, jobject object, const char *methodName, bool src);
	int CallVoidIntegerMethod(JNIEnv *env, jclass clazz, jobject object, const char *methodName, int src);

	const char *name;	// Function name;
	const char *args;

	jmethodID methodID;
};

struct JNI_Class
{
	int SetClass(JNIEnv *env, const char *className);
	int SetClassGlobal(JNIEnv *env, const char *className);

	int FindMethod(const char *methodName, const char *methodArgs);

	int CallVoidMethod(JNIEnv *env, const char *methodName, const char *methodArgs, ...);

	int CallVoidStringMethod(JNIEnv *env, const char *methodName, const char *src);
	int CallVoidBooleanMethod(JNIEnv *env, const char *methodName, bool src);
	int CallVoidIntegerMethod(JNIEnv *env, const char *methodName, int src);

    int StaticSetObjectFieldInt
                    (
                    JNIEnv *            env,
                    jobject             obj,
                    const char *        fieldNamePtr,
                    int                 val
                    );
    int StaticSetObjectFieldString
                    (
                    JNIEnv *            env,
                    jobject             obj,
                    const char *        fieldNamePtr,
                    const char *        val
                    );

    int StaticSetObjectFieldStringGBK
                (
                JNIEnv *            env,
                jobject             obj,
                const char *        fieldNamePtr,
                const char *        val
                );

    int StaticSetObjectFieldBoolean
                (
                JNIEnv *            env,
                jobject             obj,
                const char *        fieldNamePtr,
                bool                val
                );

    int StaticGetObjectFieldBoolean
                (
                JNIEnv *            env,
                jobject             obj,
                const char *        fieldNamePtr,
                bool *              val
                );
                
    int StaticGetObjectFieldInt
                (
                JNIEnv *            env,
                jobject             obj,
                const char *        fieldNamePtr,
                int *               val
                );

    int StaticGetObjectFieldString
                (
                JNIEnv *            env,
                jobject             obj,
                const char *        fieldNamePtr,
                char *              val,
                int                 maxLength
                );
        
	const char *name;	// Class name.
	jclass clazz;
	jobject object;

	jmethodID initID;

	JNI_Function functions[MAX_FUNCTIONS_NUMBER];
	int num;

	jmethodID   methodID_setObjFieldInt;
	jmethodID   methodID_setObjFieldString;
	jmethodID   methodID_setObjFieldBoolean;

	jmethodID   methodID_getObjFieldInt;
	jmethodID   methodID_getObjFieldString;
	jmethodID   methodID_getObjFieldBoolean;
	
	
};



extern "C"
{
jstring   CharTojstring(JNIEnv*   env,   const char*   str);
}



class MyJniString
{
private:
	jstring			_jstr;
	const char *	_cstr;
	JNIEnv *		_env;

public:
	MyJniString(JNIEnv *env, jstring jstr)
	{
		this->_env = env;
		this->_jstr = jstr;
		this->_cstr = 0;
	}

	virtual ~MyJniString()
	{
		if(this->_cstr)		this->_env->ReleaseStringUTFChars(this->_jstr, this->_cstr);
		if(this->_jstr)		this->_env->DeleteLocalRef(this->_jstr);
	}

	const char * cstr()
	{
		if(this->_jstr && !this->_cstr) 	this->_cstr = (this->_env)->GetStringUTFChars(this->_jstr, 0);
		return this->_cstr;
	}

};

jobject MyJni_GetSystemServiceObj(JNIEnv *env, jobject jCtxObj, const char * serviceName);
jstring MyJni_GetAndroidId(JNIEnv *env, jobject jCtxObj);
jstring MyJni_GetPhoneImei(JNIEnv *env, jobject jCtxObj);
jstring MyJni_GetWifiMacAddr(JNIEnv *env, jobject jCtxObj);


#endif

