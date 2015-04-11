 #! /bin/bash



LINE1="/Users/simon/Desktop/simon/tools/android-ndk-r10d/toolchains/arm-linux-androideabi-4.8/prebuilt/darwin-x86_64/bin/arm-linux-androideabi-g++ "
LINE1="${LINE1}  -Wl,-soname,libeice_other.so -shared --sysroot=/Users/simon/Desktop/simon/tools/android-ndk-r10d/platforms/android-19/arch-arm "
LINE1="${LINE1} ./obj/local/armeabi/objs/eice_other/dummy.o "

#LINE1="${LINE1} ./jni/libeice.a"

LINE1="${LINE1} /Users/simon/Desktop/simon/tools/android-ndk-r10d/sources/cxx-stl/stlport/libs/armeabi/thumb/libstlport_static.a "
#LINE1="${LINE1} /Users/simon/Desktop/simon/tools/android-ndk-r10d/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi/thumb/libgnustl_static.a "



LINE1="${LINE1} -lgcc -no-canonical-prefixes "
LINE1="${LINE1} -leice "
LINE1="${LINE1} -L/Users/simon/Desktop/simon/tools/android-ndk-r10d/sources/cxx-stl/stlport/libs/armeabi/thumb/ -lstlport_static"

LINE1="${LINE1} -lpjnath-arm-unknown-linux-androideabi -lpjlib-util-arm-unknown-linux-androideabi -lpj-arm-unknown-linux-androideabi "
LINE1="${LINE1} -lgnustl_static -llog "
LINE1="${LINE1} -Ljni -L/Users/simon/Desktop/simon/tools/android-ndk-r10d/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/ "
LINE1="${LINE1} -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -mthumb   -lc -lm "
LINE1="${LINE1} -o ./obj/local/armeabi/libeice_other.so"

LINE2="/Users/simon/Desktop/simon/tools/android-ndk-r10d/toolchains/arm-linux-androideabi-4.8/prebuilt/darwin-x86_64/bin/arm-linux-androideabi-g++ "
LINE2="${LINE2}  -Wl,-soname,libeice_other.so -shared --sysroot=/Users/simon/Desktop/simon/tools/android-ndk-r10d/platforms/android-19/arch-arm "
LINE2="${LINE2}  ./obj/local/armeabi/objs/eice_other/dummy.o "
LINE2="${LINE2}  /Users/simon/Desktop/simon/tools/android-ndk-r10d/sources/cxx-stl/stlport/libs/armeabi/thumb/libstlport_static.a "
LINE2="${LINE2}  -lgcc -no-canonical-prefixes "
LINE2="${LINE2}  -leice -lpjnath-arm-unknown-linux-androideabi -lpjlib-util-arm-unknown-linux-androideabi -lpj-arm-unknown-linux-androideabi "
LINE2="${LINE2}  -lgnustl_static -llog "
LINE2="${LINE2}  -Ljni -L/Users/simon/Desktop/simon/tools/android-ndk-r10d/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/ "
LINE2="${LINE2}  -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -mthumb   -lc -lm "
LINE2="${LINE2}  -o ./obj/local/armeabi/libeice_other.so"

CMD_LINE="${LINE1}"
echo ${CMD_LINE}
${CMD_LINE}
