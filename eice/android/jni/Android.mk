# $Id$

LOCAL_PATH	:= $(call my-dir)
include $(CLEAR_VARS)

# Get PJ build settings
include ../../build.mak
include $(PJDIR)/build/common.mak


MY_MODULE_PATH  := $(PJDIR)/pjsip-apps/build/output/pjsua-$(TARGET_NAME)


# Constants
EICE_SRC_PATH :=$(PJDIR)/eice/eice/src
EICE_SRC	:= eice_jni.cpp \
	$(EICE_SRC_PATH)/eice.cpp \
	$(EICE_SRC_PATH)/jsoncpp.cpp 
##	$(EICE_SRC_PATH)/MyUtilAndroidJni.cpp

# Android build settings
LOCAL_MODULE    := libeice
LOCAL_CFLAGS    := -Werror $(APP_CFLAGS) -frtti
# simon
LOCAL_CFLAGS += -fexceptions 
LOCAL_CFLAGS += -I$(EICE_SRC_PATH) 

LOCAL_SRC_FILES := $(EICE_SRC) 
LOCAL_LDFLAGS   := $(APP_LDFLAGS)
# LOCAL_LDLIBS    := $(PJLIB_LDLIB) $(PJLIB_UTIL_LDLIB) $(PJNATH_LDLIB)
LOCAL_LDLIBS    := $(APP_LDLIBS)





include $(BUILD_SHARED_LIBRARY)
