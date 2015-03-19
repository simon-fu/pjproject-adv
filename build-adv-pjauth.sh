#!/bin/sh
#

make
if [ $? -eq 0 ]; then
cp pjlib/lib/libpj-arm-apple-darwin9.a ./lib/
cp pjlib-util/lib/libpjlib-util-arm-apple-darwin9.a ./lib/
cp pjnath/lib/libpjnath-arm-apple-darwin9.a ./lib/
cp pjnath/include/eice.h ./lib/
echo "library copied to ./lib/"
exit 0
else
echo "build fail !!!"
exit -1
fi


