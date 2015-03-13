#!/bin/bash

PJLIB_PATH="./lib/$1/pjlib"
PJLIB_UTIL_PATH="./lib/$1/pjlib-util"
PJMEDIA_PATH="./lib/$1/pjmedia"
PJNATH_PATH="./lib/$1/pjnath"
PJSIP_PATH="./lib/$1/pjsip"
THIRD_PARTY="./lib/$1/third_party"

echo "start coping to $1"

rm -rf $PJLIB_PATH
rm -rf $PJLIB_UTIL_PATH
#rm -rf $PJMEDIA_PATH
rm -rf $PJNATH_PATH
#rm -rf $PJSIP_PATH
#rm -rf $THIRD_PARTY

mkdir -p $PJLIB_PATH
mkdir -p $PJLIB_UTIL_PATH
#mkdir -p $PJMEDIA_PATH
mkdir -p $PJNATH_PATH
#mkdir -p $PJSIP_PATH
#mkdir -p $THIRD_PARTY

cp -r ./pjlib/lib/ $PJLIB_PATH
cp -r ./pjlib-util/lib/ $PJLIB_UTIL_PATH
#cp -r ./pjmedia/lib/ $PJMEDIA_PATH
cp -r ./pjnath/lib/ $PJNATH_PATH
#cp -r ./pjsip/lib/ $PJSIP_PATH
#cp -r ./third_party/lib/ $THIRD_PARTY

echo "copy done"

