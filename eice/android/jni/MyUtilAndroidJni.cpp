#include "MyUtilAndroidJni.h"

#undef dlogi
#undef dlogw
#undef dloge
#undef ddump

#define dlogi(...)  __android_log_print(ANDROID_LOG_ERROR,"mtv",__VA_ARGS__)  
#define dlogw(...)  __android_log_print(ANDROID_LOG_ERROR,"mtv",__VA_ARGS__)
#define dloge(...)  __android_log_print(ANDROID_LOG_ERROR,"mtv",__VA_ARGS__)
#define ddump(...)  __android_log_print(ANDROID_LOG_ERROR,"mtv",__VA_ARGS__)


int JNI_Function::IsEqual(const char *methodName, const char *methodArgs)
{
	if (!name || !args) return -1;

	//dlogi("%s, %s VS %s, %s\n", name, args, methodName, methodArgs);

	if (0 == strcmp(name, methodName))
	{
		// Set.
		return 0;
	}

	return -1;
}

int JNI_Function::SetMethod(JNIEnv *env, jclass clazz, const char *methodName, const char *methodArgs)
{
	assert(env && clazz && methodName && methodArgs);

	if (0 == IsEqual(methodName, methodArgs))
		return 0;

	if (!(methodID = env->GetMethodID(clazz, methodName, methodArgs)))
	{
		dlogi("Can not find %s with %s.\n", methodName, methodArgs);
		return -1;
	}

	name = methodName;
	args = methodArgs;

	//dlogi("%s(%s):%p.\n", methodName, methodArgs, methodID);

	return 0;
}

int JNI_Function::CallVoidMethod(JNIEnv *env, jclass clazz, jobject object, const char *methodName, const char *methodArgs, ...)
{
	if (SetMethod(env, clazz, methodName, methodArgs))
		return -1;

	return 0;
}

int JNI_Function::CallVoidStringMethod(JNIEnv *env, jclass clazz, jobject object, const char *methodName, const char *src)
{
	if (SetMethod(env, clazz, methodName, VOID_STRING_METHOD_ARGS))
		return -1;

	jstring srcJ = NULL;
	if (! (srcJ = env->NewStringUTF(src)))
	{
		dlogi("Error when new a string %s.\n", src);
		return -1;
	}

	//dlogi("New a string %s\n", src);

	env->CallVoidMethod(object, methodID, srcJ);
	env->DeleteLocalRef(srcJ);

	return 0;
}

int JNI_Function::CallVoidBooleanMethod(JNIEnv *env, jclass clazz, jobject object, const char *methodName, bool src)
{
	if (SetMethod(env, clazz, methodName, VOID_BOOLEAN_METHOD_ARGS))
		return -1;

	//dlogi("Boolean %d\n", src);

	env->CallVoidMethod(object, methodID, src);

	return 0;
}

int JNI_Function::CallVoidIntegerMethod(JNIEnv *env, jclass clazz, jobject object, const char *methodName, int src)
{
	if (SetMethod(env, clazz, methodName, VOID_INTEGER_METHOD_ARGS))
		return -1;

	//dlogi("Integer %d\n", src);

	env->CallVoidMethod(object, methodID, src);

	return 0;
}

int JNI_Class::SetClass(JNIEnv *env, const char *className)
{
	memset(functions, 0, sizeof(functions));

	methodID_setObjFieldInt = 0;
	methodID_setObjFieldString = 0;
	methodID_setObjFieldBoolean = 0;

	methodID_getObjFieldInt = 0;
	methodID_getObjFieldString = 0;
	methodID_getObjFieldBoolean = 0;
	
	
	num = 0;

	if (!(clazz = env->FindClass(className)))
	{
		dlogi("Can not find Class %s.\n", className);
		return -1;
	}

	if (!(initID = env->GetMethodID(clazz, "<init>", "()V")))
	{
		dlogi("Can not find <init>() function of Class %s.\n", className);
		return -1;
	}

	if (!(object = env->NewObject(clazz, initID)))
	{
		dlogi("Can not create object for Class %s.\n", className);
		return -1;
	}

	name = className;

	//dlogi("%s: %p, %p, %p\n", name, clazz, initID, object);

	return 0;
}

int JNI_Class::SetClassGlobal(JNIEnv *env, const char *className)
{
    SetClass(env, className);
    clazz = (jclass) env->NewGlobalRef(clazz);
    object = (jobject) env->NewGlobalRef(object);

    return 0;
}

int JNI_Class::FindMethod(const char *methodName, const char *methodArgs)
{
	if (num > 0) // More than 1.
	{
		int index = 0;
		for (; index < num; index ++)
		{
			if (0 == functions[index].IsEqual(methodName, methodArgs))
				break;
		}

		if (index >= 0 && index < num)
			return index;
	}

	// index == num;
	if ((num + 1) >= MAX_FUNCTIONS_NUMBER)
	{
		dlogi("Can not add %s(%s) to %d.\n", methodName, methodArgs, num);
		return -1;
	}

	//dlogi("Add %s(%s) to %d.\n", methodName, methodArgs, num);

	return num ++;
}

int JNI_Class::CallVoidMethod(JNIEnv *env, const char *methodName, const char *methodArgs, ...)
{
	int index = FindMethod(methodName, methodArgs);
	if (index < 0)
		return -1;

//	functions[index].CallVoidMethod(env, clazz, object, methodName, methodArgs, ...);

	return 0;
}

int JNI_Class::CallVoidStringMethod(JNIEnv *env, const char *methodName, const char *src)
{
	int index = FindMethod(methodName, VOID_STRING_METHOD_ARGS);
	if (index < 0)
		return -1;

	return functions[index].CallVoidStringMethod(env, clazz, object, methodName, src);
}

int JNI_Class::CallVoidBooleanMethod(JNIEnv *env, const char *methodName, bool src)
{
	int index = FindMethod(methodName, VOID_BOOLEAN_METHOD_ARGS);
	if (index < 0)
		return -1;

	return functions[index].CallVoidBooleanMethod(env, clazz, object, methodName, src);
}

int JNI_Class::CallVoidIntegerMethod(JNIEnv *env, const char *methodName, int src)
{
	int index = FindMethod(methodName, VOID_INTEGER_METHOD_ARGS);
	if (index < 0)
		return -1;

	return functions[index].CallVoidIntegerMethod(env, clazz, object, methodName, src);
}


int JNI_Class::StaticSetObjectFieldInt
                (
                JNIEnv *            env,
                jobject             obj,
                const char *        fieldNamePtr,
                int                 val
                )
{
    jclass      classUtil = clazz;
    jmethodID   mid = 0;
    jstring     fieldName = 0;
    jobject     objVal = 0;
    int         ret = -1;
	
    do
    {
       	if (classUtil == 0)
       	{
       	    dloge("clazz is null\n");
    		break;
        }

        if(methodID_setObjFieldInt == 0)
        {
            methodID_setObjFieldInt = (env)->GetStaticMethodID
                                        (
                                        classUtil, 
                                        "setObjFieldInt", 
                                        "(Ljava/lang/Object;Ljava/lang/String;I)V"
                                        );
        }

        mid = methodID_setObjFieldInt;
        if(mid == 0)
        {
       	    dloge("can't get field: setObjFieldInt\n");
    		break;
        }


        fieldName = (env)->NewStringUTF(fieldNamePtr);
        //objVal = (env)->NewStringUTF(val);
        //(env)->CallStaticVoidMethod( classUtil, mid, obj, fieldName, objVal);
        //(env)->DeleteLocalRef(objVal);
        (env)->CallStaticVoidMethod( classUtil, mid, obj, fieldName, val);
        
        (env)->DeleteLocalRef(fieldName);


        ret = 0;
        
	}while(0);

	return ret;
}



int JNI_Class::StaticSetObjectFieldString
                (
                JNIEnv *            env,
                jobject             obj,
                const char *        fieldNamePtr,
                const char *        val
                )
{
    jclass      classUtil = clazz;
    jmethodID   mid = 0;
    jstring     fieldName = 0;
    jobject     objVal = 0;
    int         ret = -1;
	
    do
    {
       	if (classUtil == 0)
       	{
       	    dloge("clazz is null\n");
    		break;
        }

        if(methodID_setObjFieldString == 0)
        {
            methodID_setObjFieldString = (env)->GetStaticMethodID
                                        (
                                        classUtil, 
                                        "setObjField", 
                                        "(Ljava/lang/Object;Ljava/lang/String;Ljava/lang/Object;)V"
                                        );
        }

        mid = methodID_setObjFieldString;
        
                                    ;
        if(mid == 0)
        {
       	    dloge("can't get field: setObjField\n");
    		break;
        }


        fieldName = (env)->NewStringUTF(fieldNamePtr);
        objVal = (env)->NewStringUTF(val);
        
        (env)->CallStaticVoidMethod( classUtil, mid, obj, fieldName, objVal);
        
        (env)->DeleteLocalRef(objVal);
        (env)->DeleteLocalRef(fieldName);


        ret = 0;
        
	}while(0);

	return ret;
}


int JNI_Class::StaticSetObjectFieldStringGBK
                (
                JNIEnv *            env,
                jobject             obj,
                const char *        fieldNamePtr,
                const char *        val
                )
{
    jclass      classUtil = clazz;
    jmethodID   mid = 0;
    jstring     fieldName = 0;
    jobject     objVal = 0;
    int         ret = -1;
	
    do
    {
       	if (classUtil == 0)
       	{
       	    dloge("clazz is null\n");
    		break;
        }

        if(methodID_setObjFieldString == 0)
        {
            methodID_setObjFieldString = (env)->GetStaticMethodID
                                        (
                                        classUtil, 
                                        "setObjField", 
                                        "(Ljava/lang/Object;Ljava/lang/String;Ljava/lang/Object;)V"
                                        );
        }

        mid = methodID_setObjFieldString;
        
                                    ;
        if(mid == 0)
        {
       	    dloge("can't get field: setObjField\n");
    		break;
        }


        fieldName = (env)->NewStringUTF(fieldNamePtr);
        //objVal = (env)->NewStringUTF(val);
        objVal =   CharTojstring(env,  val ) ;
        
        (env)->CallStaticVoidMethod( classUtil, mid, obj, fieldName, objVal);
        
        (env)->DeleteLocalRef(objVal);
        (env)->DeleteLocalRef(fieldName);


        ret = 0;
        
	}while(0);

	return ret;
}






int JNI_Class::StaticSetObjectFieldBoolean
                (
                JNIEnv *            env,
                jobject             obj,
                const char *        fieldNamePtr,
                bool                val
                )
{
    jclass      classUtil = clazz;
    jmethodID   mid = 0;
    jstring     fieldName = 0;
    jobject     objVal = 0;
    int         ret = -1;
	
    do
    {
       	if (classUtil == 0)
       	{
       	    dloge("clazz is null\n");
    		break;
        }

        if(methodID_setObjFieldBoolean == 0)
        {
            methodID_setObjFieldBoolean = (env)->GetStaticMethodID
                                        (
                                        classUtil, 
                                        "setObjFieldBoolean", 
                                        "(Ljava/lang/Object;Ljava/lang/String;Z)V"
                                        );
        }

        mid = methodID_setObjFieldBoolean;
        
        if(mid == 0)
        {
       	    dloge("can't get field: setObjFieldBoolean\n");
    		break;
        }


        fieldName = (env)->NewStringUTF(fieldNamePtr);
        //objVal = (env)->NewStringUTF(val);
        //(env)->CallStaticVoidMethod( classUtil, mid, obj, fieldName, objVal);
        //(env)->DeleteLocalRef(objVal);
        (env)->CallStaticVoidMethod( classUtil, mid, obj, fieldName, val);
        
        (env)->DeleteLocalRef(fieldName);


        ret = 0;
        
	}while(0);

	return ret;
}

int JNI_Class::StaticGetObjectFieldBoolean
                (
                JNIEnv *            env,
                jobject             obj,
                const char *        fieldNamePtr,
                bool *              val
                )
{

    jclass      classUtil = clazz;
    jmethodID   mid = 0;
    jstring     fieldName = 0;
    jobject     objVal = 0;
    int         ret = -1;

	
    do
    {
       	if (classUtil == 0)
       	{
       	    dloge("clazz is null\n");
    		break;
        }

        if(methodID_getObjFieldBoolean == 0)
        {
            methodID_getObjFieldBoolean = (env)->GetStaticMethodID
                                        (
                                        classUtil, 
                                        "getObjFieldBoolean", 
                                        "(Ljava/lang/Object;Ljava/lang/String;)Z"
                                        );
        }

        mid = methodID_getObjFieldBoolean;
        
        if(mid == 0)
        {
       	    dloge("can't get field: getObjFieldBoolean\n");
    		break;
        }


        fieldName = (env)->NewStringUTF(fieldNamePtr);
        //objVal = (env)->NewStringUTF(val);
        //(env)->CallStaticVoidMethod( classUtil, mid, obj, fieldName, objVal);
        //(env)->DeleteLocalRef(objVal);
        
        *val = (env)->CallStaticBooleanMethod( classUtil, mid, obj, fieldName);
        

        (env)->DeleteLocalRef(fieldName);


        ret = 0;
        
	}while(0);

	return ret;

}


int JNI_Class::StaticGetObjectFieldInt
                (
                JNIEnv *            env,
                jobject             obj,
                const char *        fieldNamePtr,
                int *               val
                )
{

    jclass      classUtil = clazz;
    jmethodID   mid = 0;
    jstring     fieldName = 0;
    jobject     objVal = 0;
    int         ret = -1;

	
    do
    {
       	if (classUtil == 0)
       	{
       	    dloge("clazz is null\n");
    		break;
        }

        if(methodID_getObjFieldInt == 0)
        {
            methodID_getObjFieldInt = (env)->GetStaticMethodID
                                        (
                                        classUtil, 
                                        "getObjFieldInt", 
                                        "(Ljava/lang/Object;Ljava/lang/String;)I"
                                        );
        }

        mid = methodID_getObjFieldInt;
        
        if(mid == 0)
        {
       	    dloge("can't get field: getObjFieldInt\n");
    		break;
        }


        fieldName = (env)->NewStringUTF(fieldNamePtr);
        //objVal = (env)->NewStringUTF(val);
        //(env)->CallStaticVoidMethod( classUtil, mid, obj, fieldName, objVal);
        //(env)->DeleteLocalRef(objVal);
        
        *val = (env)->CallStaticIntMethod( classUtil, mid, obj, fieldName);
        

        (env)->DeleteLocalRef(fieldName);


        ret = 0;
        
	}while(0);

	return ret;

}


int JNI_Class::StaticGetObjectFieldString
                (
                JNIEnv *            env,
                jobject             obj,
                const char *        fieldNamePtr,
                char *              val,
                int                 maxLength
                )
{

    jclass      classUtil = clazz;
    jmethodID   mid = 0;
    jstring     fieldName = 0;
    jobject     objVal = 0;
    int         ret = -1;

	
    do
    {
       	if (classUtil == 0)
       	{
       	    dloge("clazz is null\n");
    		break;
        }

        if(methodID_getObjFieldString == 0)
        {
            methodID_getObjFieldString = (env)->GetStaticMethodID
                                        (
                                        classUtil, 
                                        "getObjFieldString", 
                                        "(Ljava/lang/Object;Ljava/lang/String;)Ljava/lang/String;"
                                        );
        }

        mid = methodID_getObjFieldString;
        
        if(mid == 0)
        {
       	    dloge("can't get field: getObjFieldString\n");
    		break;
        }


        fieldName = (env)->NewStringUTF(fieldNamePtr);
        //objVal = (env)->NewStringUTF(val);
        //(env)->CallStaticVoidMethod( classUtil, mid, obj, fieldName, objVal);
        //(env)->DeleteLocalRef(objVal);

        objVal = (env)->CallStaticObjectMethod( classUtil, mid, obj, fieldName);

        jstring         strobj = (jstring) objVal;
        const char *    p = env->GetStringUTFChars(strobj, NULL);
        int             len = strlen(p);

        if(len > (maxLength - 1))
        {
            len = (maxLength - 1);
        }

        memcpy(val, p, len);
        val[len] = 0;

        env->ReleaseStringUTFChars(strobj, p);//release jstring
        
        (env)->DeleteLocalRef(strobj);
        (env)->DeleteLocalRef(fieldName);


        ret = 0;
        
	}while(0);

	return ret;

}


#define   CURRENT_ENCODE   "GBK"
jstring   CharTojstring(JNIEnv*   env,   const char*   str) 
{ 
                jstring   rtn   =   0; 
                jsize   len   =   strlen(str); 

                jclass   clsstring   =   env-> FindClass( "java/lang/String"); 
                jstring   strencode = 0;
                jmethodID   mid = 0;
                jbyteArray   barr = 0;

        do
        {
                if(!clsstring)
                {
                    dloge("clsstring 0 *****\n");
                    break;
                }
                
                //new   encode   string   default   "GBK " 
                strencode   =   env-> NewStringUTF(CURRENT_ENCODE); 
                if(!strencode)
                {
                    dloge("strencode 0 *****\n");
                    break;
                }
                
                mid   =   env-> GetMethodID(clsstring,   "<init>",   "([BLjava/lang/String;)V"); 
                if(!mid)
                {
                    dloge("mid 0 *****\n");
                    break;
                }
                
                barr   =   env-> NewByteArray(len); 
                if(!barr)
                {
                    dloge("barr 0 *****\n");
                    break;
                }

                
                env-> SetByteArrayRegion(barr,0,len,(jbyte*)str); 

                //call   new   String(byte[]   b,String   encode) 
                rtn   =   (jstring)env-> NewObject(clsstring,mid,barr,strencode); 
        }while(0);

        if(barr)
        {
            (env)->DeleteLocalRef(barr);
            barr = 0;
        }

        if(strencode)
        {
            (env)->DeleteLocalRef(strencode);
            strencode = 0;
        }
        

        return   rtn; 
}



#if 0

// jclass and jobject should be free by env->DeleteLocalRef

jobject MyJni_GetSystemServiceObj(JNIEnv *env, jobject jCtxObj, const char * serviceName)
{
		jclass jCtxClz= env->FindClass("android/content/Context");
		jfieldID jfid_service = env->GetStaticFieldID(jCtxClz, serviceName,"Ljava/lang/String;");
		jstring  jstr_serveice = (jstring)env->GetStaticObjectField(jCtxClz, jfid_service);
		jmethodID  mid_getSystemService = env->GetMethodID(jCtxClz,"getSystemService","(Ljava/lang/String;)Ljava/lang/Object;");
		jobject jserviceObj = env->CallObjectMethod(jCtxObj,mid_getSystemService, jstr_serveice);
		env->DeleteLocalRef(jCtxClz);
		env->DeleteLocalRef(jstr_serveice);
		return jserviceObj;
}

jstring MyJni_GetAndroidId(JNIEnv *env, jobject jCtxObj)
{
	// String androidId = Secure.getString(context.getContentResolver(), Secure.ANDROID_ID);
	jclass		claz = 0;
	jstring		jstr_ANDROID_ID = 0;
	jobject		resolver = 0;
	jstring		jId = 0;
	do
	{
		// resolver = jCtxObj.getContentResolver()
		claz = (env)->FindClass( "android/content/Context");
		jmethodID getMethod = (env)->GetMethodID(claz, "getContentResolver", "()Landroid/content/ContentResolver;");
		resolver = (env)->CallObjectMethod(jCtxObj, getMethod);
		if (!resolver) 
		{
			//dbge("invalid resolver!");
			break;
		}
		env->DeleteLocalRef(claz);
		claz = 0;

		
		// claz = Settings.Secure
		claz = (env)->FindClass("android/provider/Settings$Secure");
		if (claz == NULL) {
			//dbge("invalid Secure class!");
			break;
		}

		//Secure.ANDROID_ID
		jfieldID field_ANDROID_ID = (env)->GetStaticFieldID(claz, "ANDROID_ID", "Ljava/lang/String;");
		jstr_ANDROID_ID = (jstring)((env)->GetStaticObjectField(claz, field_ANDROID_ID));

		// getStringMethod = Secure.getString
		jmethodID getStringMethod = (env)->GetStaticMethodID(claz, "getString", "(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;");
		if (getStringMethod == NULL) {
			//dbge("Invalid getStringMethod!");
			break;
		}

		// jId = claz.getStringMethod(resolver, str);
		jId = (jstring)((env)->CallStaticObjectMethod(claz, getStringMethod, resolver, jstr_ANDROID_ID));

	}while(0);

	if(jstr_ANDROID_ID) env->DeleteLocalRef(jstr_ANDROID_ID);
	if(resolver) env->DeleteLocalRef(resolver);
	if(claz) env->DeleteLocalRef(claz);
	
	return jId;
}



jstring MyJni_GetPhoneImei(JNIEnv *env, jobject jCtxObj)
{
		//TelephonyManager m = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
	    //String imei = m != null ? m.getDeviceId() : null;
	jobject		serviceObj = 0;
	jobject		wifiInfoObj = 0;
	jmethodID	mid = 0;
	jstring		jstr = 0;
	do
	{
		serviceObj = MyJni_GetSystemServiceObj(env, jCtxObj, "TELEPHONY_SERVICE");
		if(!serviceObj) break;

		jclass jclzTelMgr = env->GetObjectClass(serviceObj);
		mid = env->GetMethodID(jclzTelMgr,"getDeviceId","()Ljava/lang/String;");
		jstr = (jstring) env->CallObjectMethod(serviceObj, mid);
		env->DeleteLocalRef(jclzTelMgr);

	}while(0);

	if(serviceObj) env->DeleteLocalRef(serviceObj);

	return jstr;

}

jstring MyJni_GetWifiMacAddr(JNIEnv *env, jobject jCtxObj)
{
	    //WifiManager wm = (WifiManager)context.getSystemService(Context.WIFI_SERVICE); 
	    //String macaddr = wm.getConnectionInfo().getMacAddress();

	jobject		wifiManagerObj = 0;
	jobject		wifiInfoObj = 0;
	jmethodID	mid = 0;
	jstring		jstrMac = 0;
	do
	{
		wifiManagerObj = MyJni_GetSystemServiceObj(env, jCtxObj, "WIFI_SERVICE");
		if(!wifiManagerObj) break;

		jclass jclzWifiMgr = env->GetObjectClass(wifiManagerObj);
		mid = env->GetMethodID(jclzWifiMgr,"getConnectionInfo","()Landroid/net/wifi/WifiInfo;");
		wifiInfoObj = env->CallObjectMethod(wifiManagerObj,mid);
		env->DeleteLocalRef(jclzWifiMgr);
		if(!wifiInfoObj) break;

		jclass jclzWifiInfo = env->GetObjectClass(wifiInfoObj);
		mid = env->GetMethodID(jclzWifiInfo,"getMacAddress","()Ljava/lang/String;");
		jstrMac = (jstring)env->CallObjectMethod(wifiInfoObj,mid);
		env->DeleteLocalRef(jclzWifiInfo);

	}while(0);

	if(wifiManagerObj) env->DeleteLocalRef(wifiManagerObj);
	if(wifiInfoObj) env->DeleteLocalRef(wifiInfoObj);

	return jstrMac;
}
#endif

