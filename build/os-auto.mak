# build/os-auto.mak.  Generated from os-auto.mak.in by configure.

export OS_CFLAGS   := $(CC_DEF)PJ_AUTOCONF=1 -O2 -Wno-unused-label -DPJ_SDK_NAME="\"iPhoneOS8.1.sdk\"" -arch arm64 -isysroot /Applications/XCode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS8.1.sdk -DPJ_IS_BIG_ENDIAN=0 -DPJ_IS_LITTLE_ENDIAN=1

export OS_CXXFLAGS := $(CC_DEF)PJ_AUTOCONF=1 -O2 -Wno-unused-label -DPJ_SDK_NAME="\"iPhoneOS8.1.sdk\"" -arch arm64 -isysroot /Applications/XCode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS8.1.sdk 

export OS_LDFLAGS  := -O2 -arch arm64 -isysroot /Applications/XCode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS8.1.sdk -framework AudioToolbox -framework Foundation -lm -lpthread  -framework CoreAudio -framework CoreFoundation -framework AudioToolbox -framework CFNetwork -framework UIKit -framework UIKit -framework OpenGLES -framework AVFoundation -framework CoreGraphics -framework QuartzCore -framework CoreVideo -framework CoreMedia

export OS_SOURCES  := 


