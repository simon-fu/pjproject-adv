# build/os-auto.mak.  Generated from os-auto.mak.in by configure.

export OS_CFLAGS   := $(CC_DEF)PJ_AUTOCONF=1  -I/Users/simon/Desktop/simon/tools/android-ndk-r10d/platforms/android-19/arch-arm/usr/include -I/Users/simon/Desktop/simon/tools/android-ndk-r10d/sources/cxx-stl/gnu-libstdc++/4.9/include -I/Users/simon/Desktop/simon/tools/android-ndk-r10d/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/include -DPJ_IS_BIG_ENDIAN=0 -DPJ_IS_LITTLE_ENDIAN=1

export OS_CXXFLAGS := $(CC_DEF)PJ_AUTOCONF=1  -I/Users/simon/Desktop/simon/tools/android-ndk-r10d/platforms/android-19/arch-arm/usr/include -I/Users/simon/Desktop/simon/tools/android-ndk-r10d/sources/cxx-stl/gnu-libstdc++/4.9/include -I/Users/simon/Desktop/simon/tools/android-ndk-r10d/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/include  -shared --sysroot=/Users/simon/Desktop/simon/tools/android-ndk-r10d/platforms/android-19/arch-arm -fexceptions -frtti

export OS_LDFLAGS  :=  -nostdlib -L/Users/simon/Desktop/simon/tools/android-ndk-r10d/platforms/android-19/arch-arm/usr/lib/ -L/Users/simon/Desktop/simon/tools/android-ndk-r10d/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/ -lm /Users/simon/Desktop/simon/tools/android-ndk-r10d/platforms/android-19/arch-arm/usr/lib/crtbegin_so.o -lgnustl_static  -lc -lgcc -lOpenSLES -llog

export OS_SOURCES  := 


