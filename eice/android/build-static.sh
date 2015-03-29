#!/bin/sh

echo $@
ndk-build LIB_TYPE=static $@

