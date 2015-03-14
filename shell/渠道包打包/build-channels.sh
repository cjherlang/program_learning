#!/bin/bash
#cd `dirname $0`

if [ "$#" -lt "1" ]; then
    echo "must give a apk for channel build!"
    exit
fi

OFFICIAL_APK=$1
APK_TYPE="-f"
PRODUCT_TYPE="-zjh"

for arg in "$@";
do
   if [ "$arg" = "-f" ] || [ "$arg" = "-s" ]; then 
	APK_TYPE=$arg
   fi
   if [ "$arg" = "-zjh" ] || [ "$arg" = "-dn" ] || [ "$arg" = "-lhj" ] || [ "$arg" = "-ft" ]; then
	PRODUCT_TYPE=$arg
   fi
done

#SCRIPT_DIR=`pwd`
#SCRIPT_DIR="d:/game/android_channel"
SCRIPT_DIR=`readlink -e $0`
SCRIPT_DIR=`dirname $SCRIPT_DIR`

source ${SCRIPT_DIR}/functions.sh
if [ "${PRODUCT_TYPE}" = "-zjh" ]; then
   echo "use init-vars-game"
   source ${SCRIPT_DIR}/init-vars-game.sh
else
   echo "use init-vars-bgame"
   source ${SCRIPT_DIR}/init-vars-bgame.sh
fi

CHANNELS="${SCRIPT_DIR}/channels${PRODUCT_TYPE}.txt"

echo "OFFICIAL_APK is : ${OFFICIAL_APK}"
echo "APK_TYPE is : ${APK_TYPE}"
echo "PRODUCT_TYPE is : ${PRODUCT_TYPE}"
echo "CHANNELS is : ${CHANNELS}"
#exit

APKTOOL="java -jar ${SCRIPT_DIR}/apktool.jar"
#ZIPALIGN="${SCRIPT_DIR}/zipalign-Darwin"
ZIPALIGN="zipalign"

APP_NAME=`echo $OFFICIAL_APK | awk -F"." '{print $1}'`

#LAST_ADDRESSE="gs2.starfieldgame.com:7011"
CURRENT_ADDRESS="gs2.starfieldgame.com:7011"

BUILDING_DIR="${SCRIPT_DIR}/tmp/zjh_channels/workspace"
UNSIGNED_DIR="${SCRIPT_DIR}/tmp/zjh_channels/unsigned"
SIGNED_DIR="${SCRIPT_DIR}/channels"

build_apk() {
    local marketName="$1"
    local marketId="$2"
    if [ "${APK_TYPE}" = "-s" ]; then
         marketId="s_$2"
    fi

    echo "building apk for $marketName $marketId ..."

    #update_address

    update_channel
   
    #update_unipayChannel

    local unsigned_file="${UNSIGNED_DIR}/${marketId}-${marketName}-${APP_NAME}.apk"
    local signed_file="${SIGNED_DIR}/${marketId}-${marketName}-${APP_NAME}.apk"
    $APKTOOL b "$BUILDING_DIR" "$unsigned_file"
    pushd "${SCRIPT_DIR}"
    aapt a "$unsigned_file" CopyrightDeclaration.xml mmiap.xml VERSION
    popd
    sign_apk "$unsigned_file" "$signed_file"
}

update_address() {
    local s="${BUILDING_DIR}/assets/ServerAddress"
    sed -i -e "s/${LAST_ADDRESSE}/${CURRENT_ADDRESS}/g" "$s"
    fgrep "${CURRENT_ADDRESS}" "$s"
    check_return "build_apk: replace SERVERADDRESS for ${CURRENT_ADDRESS} failed."
    LAST_ADDRESSE=${CURRENT_ADDRESS}
}

update_channel() {
    local i="${BUILDING_DIR}/assets/ChannelId"
    sed -i -e "s/${LAST_MARKET}/${marketId}/g" "$i"
    fgrep "${marketId}" "$i"
    check_return "build_apk: replace marketId for ${marketName} ${marketId} failed."

    local m="${BUILDING_DIR}/AndroidManifest.xml"
    sed -i -e "s/${LAST_MARKET}/${marketId}/g" "$m"
    fgrep "${marketId}" "$m"
    check_return "build_apk: replace androidmanifest marketId for ${marketName} ${marketId} failed."
    LAST_MARKET=${marketId}
}

update_unipayChannel() {
    unipayId="00013531"
    local p="${BUILDING_DIR}/assets/premessable.txt"
    sed -i -e "s/${LAST_UNIPAYID}/${unipayId}/g" "$p"
    fgrep "${unipayId}" "$p"
    check_return "build_apk: replace unipayid for ${unipayId} failed."
}

sign_apk() {
    local apk="$1"
    local target_apk="$2"
    local pre_apk="${target_apk}.prezipalign"
    if [ ! -e "$apk" ] ; then
        echo "sign_apk: apk ${apk} does not exist."
        return 1
    fi

    echo "sign_apk: signing ${apk} ..."
    jarsigner -verbose -sigalg MD5withRSA -digestalg SHA1 \
            -keystore "${KEY_STORE}" -storepass "${KEY_STORE_PASS}" \
            -keypass "${KEY_ALIAS_PASS}" -signedjar "${pre_apk}" "${apk}" "${KEY_ALIAS}"

    echo "sign_apk: zipalign ${target_apk} ..."
    $ZIPALIGN -f -v 4 "${pre_apk}" "${target_apk}"
    rm "${pre_apk}"
}

BUILD_COUNT=0
clean_dir "${SIGNED_DIR}"

while read line ; do
    if [ $(($BUILD_COUNT%10)) -eq 0 ] ; then
    echo -e "\n>>>>>>>>>>>>>>>load recourse!!!\n"
    LAST_MARKET="mmarket"
    LAST_ADDRESSE="gs2.starfieldgame.com:7011"
    LAST_UNIPAYID="00012243"
    clean_dir "${BUILDING_DIR}"
    clean_dir "${UNSIGNED_DIR}"

    $APKTOOL d -s -f "${OFFICIAL_APK}" "${BUILDING_DIR}"
    check_return "apktool: extract package."
    fi
    BUILD_COUNT=$(($BUILD_COUNT + 1))

    if [ "" = "${line}" ] ; then
       continue;
    fi
    echo -e "\n"$BUILD_COUNT">>>>>>>>>>>>>>>"$line"\n"
    market_name=`echo $line | awk '{print $1}'`
    market_id=`echo $line | awk '{print $2}'`
    build_apk "$market_name" "$market_id"
done < "${CHANNELS}"
#build_apk "天翼CTEStore" "CTEStore"
#build_apk "移动MMarket" "mmarket"
#build_apk "鼎为" "dingwei"
#build_apk "德晨" "dechen"
