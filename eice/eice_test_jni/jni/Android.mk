

# $Id$

LOCAL_PATH	:= $(call my-dir)
include $(CLEAR_VARS)


LOCAL_MODULE    := libeice_other

LOCAL_CFLAGS := -Werror -frtti -fexceptions 


MY_CFLAGS :=
MY_CFLAGS += -I/Users/simon/Desktop/simon/tools/android-ndk-r10d/sources/cxx-stl/gnu-libstdc++/4.8/include 
MY_CFLAGS += -I/Users/simon/Desktop/simon/tools/android-ndk-r10d/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi/include 
MY_CFLAGS += -I/Users/simon/Desktop/simon/tools/android-ndk-r10d/sources/cxx-stl/gnu-libstdc++/4.8/include/backward
MY_CFLAGS += -I/Users/simon/Desktop/simon/tools/android-ndk-r10d/platforms/android-19/arch-arm/usr/include 
MY_CFLAGS += -DNDEBUG
#LOCAL_CFLAGS += $(MY_CFLAGS)
LOCAL_CFLAGS += -DEICE_STATIC

# LOCAL_SRC_FILES	+= test_jni.c 
LOCAL_SRC_FILES += dummy.cpp
#LOCAL_SRC_FILES += eice_jni.cpp
#LOCAL_SRC_FILES += eice.cpp

$(info >>>>>>>>>>)
$(info LOCAL_PATH=$(LOCAL_PATH))
$(info NDK_MODULE_PATH=$(NDK_MODULE_PATH))
$(info ANDROID_NDK_ROOT=$(ANDROID_NDK_ROOT))

$(info TARGET_ARCH=$(TARGET_ARCH))
$(info TARGET_PLATFORM=$(TARGET_PLATFORM))
$(info TARGET_ARCH_ABI=$(TARGET_ARCH_ABI))
$(info TARGET_ABI=$(TARGET_ABI))

$(info <<<<<<<<<<)


MY_LIB_PATH :=$(LOCAL_PATH)/
#MY_LIB_PATH :=./

MY_LIBS :=
MY_LIBS += $(MY_LIB_PATH)libeice.a 
MY_LIBS += $(MY_LIB_PATH)libpjnath-arm-unknown-linux-androideabi.a 
MY_LIBS += $(MY_LIB_PATH)libpjlib-util-arm-unknown-linux-androideabi.a 
MY_LIBS += $(MY_LIB_PATH)libpj-arm-unknown-linux-androideabi.a 

MY_LDLIBS :=
MY_LDLIBS += -leice
# MY_LDLIBS += -l_rtp
MY_LDLIBS += -lpjnath-arm-unknown-linux-androideabi
MY_LDLIBS += -lpjlib-util-arm-unknown-linux-androideabi
MY_LDLIBS += -lpj-arm-unknown-linux-androideabi 

MY_LDLIBS += -L$(ANDROID_NDK_ROOT)/sources/cxx-stl/stlport/libs/armeabi/ -lstlport_static 

# MY_LDLIBS += /Users/simon/Desktop/simon/tools/android-ndk-r10d/platforms/android-19/arch-arm/usr/lib/crtbegin_so.o 
# MY_LDLIBS += -lgnustl_static 
# MY_LDLIBS +=  -lgcc


# LOCAL_LDLIBS := $(MY_LIBS)
# LOCAL_SRC_FILES += $(MY_LIBS)




# LOCAL_CFLAGS += -lgnustl_static -lc -lgcc -ldl -lOpenSLES -llog -lc -lm 
# LOCAL_LDFLAGS := -I$(LOCAL_PATH) -lgnustl_static -lc -lgcc -ldl -lOpenSLES -llog -lc -lm -lstdc++ 
LOCAL_LDFLAGS :=  $(MY_LDLIBS) -llog -L$(LOCAL_PATH) -L$(ANDROID_NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/
   
include $(BUILD_SHARED_LIBRARY)
# include $(BUILD_STATIC_LIBRARY)


