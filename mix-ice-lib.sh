#!/bin/bash

LIB_ROOT_PATH="./lib"
OUPUT_PATH="$LIB_ROOT_PATH/mixed/"
XLIPO="/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/lipo"  

rm -rf $OUPUT_PATH
mkdir -p $OUPUT_PATH

for dir in $LIB_ROOT_PATH/armv7/*
do
    LIB_NAME=${dir##*/}
#    LIB_OUTPUT=${OUPUT_PATH}$LIB_NAME
#    rm -rf $LIB_OUTPUT
#    mkdir -p $LIB_OUTPUT
    LIB_OUTPUT=${OUPUT_PATH}

    echo "dir=$dir"
    echo "LIB_NAME=$LIB_NAME"
    echo "LIB_OUTPUT=$LIB_OUTPUT"

    for subdir in ${dir}/*
    do
        AFILE=${subdir##*/}
        ARMV7_FILE="$LIB_ROOT_PATH/armv7/$LIB_NAME/$AFILE"
        ARMV7S_FILE="$LIB_ROOT_PATH/armv7s/$LIB_NAME/$AFILE"
        ARM64_FILE="$LIB_ROOT_PATH/arm64/$LIB_NAME/$AFILE"
        I386_FILE="$LIB_ROOT_PATH/i386/$LIB_NAME/$AFILE"
        X86_64_FILE="$LIB_ROOT_PATH/x86_64/$LIB_NAME/$AFILE"

        echo "subdir=$subdir"
        echo "start mixing file: $AFILE"
        
        ${XLIPO} -arch armv7 $ARMV7_FILE -arch armv7s $ARMV7S_FILE -arch arm64 $ARM64_FILE -arch i386 $I386_FILE -arch x86_64 $X86_64_FILE -create -output ${LIB_OUTPUT}/$AFILE

    done
done

echo "all mixed done"

