#!/bin/bash

function showhelp() {
    echo "Usage: build-pro <apk_file> [apk_type] [product_type]"
    echo "<apk_file>: build apk file name"
    echo "[apk_type]: -f | -s, if not add ,default is -f(only lite apk use -s)"
    echo "[product_type]: -zjh | -dn | -lhj | -ft etc...(default is -zjh)"
}

if [ "$#" -lt "1" ]; then
    showhelp
    exit
fi

CURRENT_DIR=`pwd`
SCRIPT_DIR=`readlink -e $0`
SCRIPT_DIR=`dirname ${SCRIPT_DIR}`
CHANNELS_DIR="${CURRENT_DIR}/channels"

APK_FILE=$1

if [ ! -f ${SCRIPT_DIR}/$APK_FILE ] ; then

echo "copy to" ${SCRIPT_DIR}/$APK_FILE
cp -f $APK_FILE ${SCRIPT_DIR}/$APK_FILE

fi

source ${SCRIPT_DIR}/build-channels.sh $@

if [ -f ${SCRIPT_DIR}/${APK_FILE} ] ; then

rm ${SCRIPT_DIR}/${APK_FILE}

fi

if [ ! -d ${CHANNELS_DIR} ] ; then

mkdir ${CHANNELS_DIR}

fi

echo 'move channels apk from build dir to bak dir'
mv -f ${SCRIPT_DIR}/channels/* ${CHANNELS_DIR}/
