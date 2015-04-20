#!/bin/bash

set -e

if [ $# -lt 7 ]; then
    echo "too few parameter"
    exit 1
fi

#project name for git clone
PROJECT_NAME=$1

#abbreviation of game name
GAME_CODE=$2

#date of running job
RUNNING_DATE=$3

#should pack texture
PACK_TEXTURE=$4

#new version name
NEW_VERSION_NAME=$5

#should compress packaged texture
IS_COMPRESS=$6

#server ip and port config
IP_PORT_CONFIG=$7

#game root path
ROOT_PATH=`pwd`

#build_pro.sh path
BUILD_CHANNEL_PATH="/home/buildbot/dev/android-channel"

#root path for storing apk and channels
CHANNEL_ROOT_PATH="/home/buildbot/channel_build"

#version config root
VERSION_CONFIG_PATH="/home/buildbot/jenkins/workspace/version_config/$GAME_CODE"

echo "read environmental variable"
. /home/buildbot/.bashrc

#log fole path
LOG_FILE=${ROOT_PATH}/log_file.txt

#parameters of config file
VERSION_NAME=""
PACKAGE_NAME=""
ICON_NAME=""
IMAGES_NAME=""
APP_NAME=""
SMS_PROVIDER=""
THIRD_PROVIDER=""
SHOP_ITEM_FILTER=""
CHANNEL_NAME=""
user_param=""

if [ -f "user_param.txt" ]; then
    user_param=`cat user_param.txt`
fi

if [ -f "${LOG_FILE}"]; then
    rm -f ${LOG_FILE}
fi

#copy files
COPY_FILE()
{
    local current_path=`pwd`
    cd $ROOT_PATH/$PROJECT_NAME/game_client/proj.android
    local ip_value=`cat assets/game.properties | grep "game.server" | cut -d "=" -f 2`
    echo "ip_value is $ip_value"
    local port_value=`cat assets/game.properties | grep "game.port" | cut -d "=" -f 2`
    echo "port_value is $port_value"
    local ipAndPort=${ip_value}_${port_value}
    
    TARGET_PATH=`BUILD_APK_PATH`
    TARGET_PATH="$CHANNEL_ROOT_PATH/$TARGET_PATH/$ipAndPort"
    echo "TARGET_PATH=$TARGET_PATH"
    
    if [ -d "$TARGET_PATH" ];then
        echo "conflict path $TARGET_PATH"
        exit 1
    else
        mkdir -p "$TARGET_PATH"
    fi
    
    
    cp -r ../dist/android/*  "$TARGET_PATH/"
    if [ -f "assets/VERSION" ]; then
        COMMIT_ID=`cat assets/VERSION`
    fi      

    cd $TARGET_PATH
    
    echo "VERSION_NAME=$VERSION_NAME" >> README.txt
    echo "PACKAGE_NAME=$PACKAGE_NAME" >> README.txt
    echo "ICON_NAME=$ICON_NAME" >> README.txt
    echo "IMAGES_NAME=$IMAGES_NAME" >> README.txt
    echo "APP_NAME=$APP_NAME" >> README.txt
    echo "SMS_PROVIDER=$SMS_PROVIDER" >> README.txt
    echo "THIRD_PROVIDER=$THIRD_PROVIDER" >> README.txt
    echo "COMMIT_ID=$COMMIT_ID" >> README.txt
    echo "SAVE_PATH=$TARGET_PATH" >> README.txt
    echo "SERVER_IP=$ip_value" >> README.txt
    echo "SERVER_port=$port_value" >> README.txt
    echo " " >> README.txt

    echo "USER_PARAM=$user_param" >> README.txt
    echo " " >> README.txt

    local payment_config=""
    payment_config=`cat ${ROOT_PATH}/${PROJECT_NAME}/game_client/proj.android/assets/game.properties`
    if [ -n "$payment_config" ]; then
        echo "#payment config" >> README.txt
        echo "$payment_config" | grep "payment_notify_id" >> README.txt
        echo "$payment_config" | grep "shop_item_filter" >> README.txt
        echo "$payment_config" | grep "dpay_app_id" >> README.txt
        echo " " >> README.txt
    fi
    cd $current_path
}

#build release and channels
BUILD_RELEASE()
{
    local TEMP_PATH=`pwd`
    cd $ROOT_PATH/$PROJECT_NAME
    
    echo "update project"
    cd game_client_framework
    if [ -f update_project.sh ]; then
        chmod 777 update_project.sh
        ./update_project.sh
    fi
    cd ..
    
    cd game_protocol_base
    if [ -f update_project.sh ]; then
        chmod 777 update_project.sh
        ./update_project.sh
    fi
    cd ..
    
    cd game_client
    if [ -f update_project.sh ]; then
        chmod 777 update_project.sh
        ./update_project.sh
    fi
    
    if [ -f "../game_client_framework/tools/check_project_properties.sh" ];then
        chmod 777 ../game_client_framework/tools/check_project_properties.sh
        ../game_client_framework/tools/check_project_properties.sh ../.
    fi
	
    echo "ant clean"
    cd proj.android
    ant clean
    
    if [ -d "../dist" ];then
        rm -rf ../dist
    fi
    
    if [ -d "obj" ];then
        rm -rf obj
    fi
    if [ -d "bin" ];then
        rm -rf bin
    fi
    if [ -d "gen" ];then
        rm -rf gen
    fi
    
    echo "build apk"
    chmod 777 set_env.sh
    chmod 777 set_env_local.sh
    chmod 777 get_git_version.sh
    chmod 777 build_native.sh
    chmod 777 build_apk.sh

    if [ ! $CHANNEL_NAME = "" ]; then
        local line_channelID=`cat ./assets/ChannelId`
        sed -i -e "s/$line_channelID/$CHANNEL_NAME/g" ./assets/ChannelId
        name_channelID=`cat ./AndroidManifest.xml | grep -A1 "TDGA_CHANNEL_ID" | grep "value" | cut -d "\"" -f 2`
        if [ -z $name_channelID ]; then
            echo "no TDGA_CHANNEL_ID"
            exit 1
        else
            sed -i -e "s/\"$name_channelID\"/\"$CHANNEL_NAME\"/g" ./AndroidManifest.xml
        fi
        echo "change channel id to $CHANNEL_NAME"
    fi

    echo "ip_port_config is $IP_PORT_CONFIG"
    if [ "$IP_PORT_CONFIG" = "" ]; then
        echo "ip config is NULL"
        ./build_apk.sh $user_param
        COPY_FILE
    else
        arr=(${IP_PORT_CONFIG//;/ })
        for config in ${arr[@]}
        do
            result=`echo $config | grep \:`
            if [ "$result" = "" ] ; then
            echo "error ip and port config"
                exit 1
            fi
        done
        
        for config in ${arr[@]}
        do
            ip=`echo $config | cut -d ":" -f 1 `
            port=`echo $config | cut -d ":" -f 2`
            echo "ip is $ip"
            echo "port is $port"
            if [ ! "$ip" = "" ]; then
                old_server=`cat assets/game.properties | grep "game.server="`
                sed -i -e "s/$old_server/game.server=$ip/g" assets/game.properties
            fi
            if [ ! "$port" = "" ]; then
                old_port=`cat assets/game.properties | grep "game.port"`
                sed -i -e "s/$old_port/game.port=$port/g" assets/game.properties
            fi
            ./build_apk.sh $user_param
            COPY_FILE
        done        
    fi
    
    cd $TEMP_PATH
}

BUILD_VERSION_PARAM()
{
    local RESULT_PARAM=""
    
    if [ -z "$VERSION_NAME" ]; then
        echo "please set version_name"
        exit 1
    else
        RESULT_PARAM=" -v $VERSION_NAME"
    fi
    
    if [ -n "$PACKAGE_NAME" ]; then
        RESULT_PARAM="$RESULT_PARAM -p $PACKAGE_NAME"
    fi
    
    if [ -n "$ICON_NAME" ]; then
        RESULT_PARAM="$RESULT_PARAM -i $ICON_NAME"
    fi
    
    if [ -n "$IMAGES_NAME" ]; then
        RESULT_PARAM="$RESULT_PARAM -m $IMAGES_NAME"
    fi
    
    if [ -n "$APP_NAME" ]; then
        RESULT_PARAM="$RESULT_PARAM -a $APP_NAME"
    fi

    if [ -n "$SHOP_ITEM_FILTER" ]; then
        RESULT_PARAM="$RESULT_PARAM -f $SHOP_ITEM_FILTER"
    fi
    
    echo $RESULT_PARAM
}

BUILD_PAYMENT_PARAM()
{
    local RESULT_PARAM=""
    
    if [ -z "$VERSION_NAME" ]; then
        echo "please set version_name"
        exit 1
    else
        RESULT_PARAM=" -g $VERSION_NAME"
    fi
    
    if [ -z "$SMS_PROVIDER" ]; then
        echo "please set sms_provider"
        exit 1
    else
        RESULT_PARAM="$RESULT_PARAM -s $SMS_PROVIDER"
    fi
    
    if [ -z "$THIRD_PROVIDER" ]; then
        echo "please set third_provider"
        exit 1
    else
        RESULT_PARAM="$RESULT_PARAM -t $THIRD_PROVIDER"
    fi
    
    echo $RESULT_PARAM
}

BUILD_APK_PATH()
{
    local CURRENT_PATH=`pwd`
    cd $ROOT_PATH/$PROJECT_NAME
    local RESULT_PATH=""
    local TEMP_VAR=""
    
	TEMP_VAR=""
    TEMP_VAR=`cat game_client/proj.android/assets/VERSION`
    TEMP_VAR="$RUNNING_DATE"
    RESULT_PATH="$TEMP_VAR"

    TEMP_VAR=`cat game_client/proj.android/AndroidManifest.xml | \
                grep "android:versionName" | cut -d \" -f 2`
    RESULT_PATH="release_for_test/$GAME_CODE/$RESULT_PATH/$TEMP_VAR/$VERSION_NAME"
    
    TEMP_VAR=""
    TEMP_VAR=`cat game_client/proj.android/AndroidManifest.xml | \
                grep "package=\"" | cut -d \" -f 2`
    RESULT_PATH="$RESULT_PATH/$TEMP_VAR"
    
    TEMP_VAR=""
    ARR=(${THIRD_PROVIDER//+/ })
    for i in ${ARR[@]}
    do
        echo "i=$i" >> ${LOG_FILE}
        LINE=`cat game_protocol_base/config_libpayment/enum_to_provider.txt | grep "_${i}=" | cut -d = -f 2`
        echo "LINE=$LINE" >> ${LOG_FILE}
        if [ -z "$LINE" ]; then
            echo "invalid third provider, please check enum_to_provider.txt"
            exit 1
        fi
        if [ -z "$TEMP_VAR" ]; then
            TEMP_VAR=$LINE
        else
            TEMP_VAR=${TEMP_VAR}_$LINE
        fi
    done
    RESULT_PATH="$RESULT_PATH/$TEMP_VAR"
    echo "RESULT_PATH=${RESULT_PATH}" >> ${LOG_FILE}    

    cd $CURRENT_PATH
    
    echo $RESULT_PATH
}

RESET_PROJECT()
{
    local TEMP_PATH=`pwd`
    cd $ROOT_PATH
    cd $PROJECT_NAME
    git reset --hard HEAD
    git submodule foreach git reset --hard HEAD
    
    cd game_client/proj.android
    old_version_name=`cat AndroidManifest.xml | grep "android:versionName" | cut -d "\"" -f 2`
    old_version_code=`cat AndroidManifest.xml | grep "android:versionCode" | cut -d "\"" -f 2`
    if [ ! "$NEW_VERSION_NAME" = "" ]; then
        sed -i -e "s/\"$old_version_name\"/\"$NEW_VERSION_NAME\"/g" AndroidManifest.xml
	    NEW_VERSION_NAME=`echo $NEW_VERSION_NAME | sed "s/\.//g"`
        sed -i -e "s/\"${old_version_code}\"/\"${NEW_VERSION_NAME}\"/g" AndroidManifest.xml
    echo now version_name is `cat AndroidManifest.xml | grep "android:versionName" | cut -d "\"" -f 2`
    fi

    cd $TEMP_PATH
}

#pack texture
DO_PACK_TEXTURE()
{
	local TEMP_PATH=`pwd`
	cd $ROOT_PATH
	cd $PROJECT_NAME
	cd game_client/proj.win32
	if [ -f rewrite.sh ] && [ -f package.sh ]; then
		rm -rf ../Resources/images/package
		chmod 777 rewrite.sh
		chmod 777 package.sh
		./rewrite.sh
	fi
    if [ -f png_compress.sh ]; then
        chmod 777 png_compress.sh
        if [ $IS_COMPRESS = true ]; then
            ./png_compress.sh
        fi
    fi
	cd $TEMP_PATH
}

if [ ! -f "$PROJECT_NAME/game_client/build_version/build_version.sh" ]; then
    echo "please copy this shell to the same path of game project and make sure there is build_version.sh"
    exit 1
fi

if [ ! -f "$PROJECT_NAME/game_protocol_base/config_libpayment/build_payment.sh" ]; then
    echo "please copy this shell to the same path of game project and make sure there is build_payment.sh"
    exit 1
fi

#load config file
FLAG=1
ORIGINAL_PATH=`pwd`

if [ ! -f "file_para/version_payment_param.txt" ]; then
    echo "please upload config file"
    exit 1
fi

fromdos "file_para/version_payment_param.txt"
if [ -f "file_para/temp_param.txt" ]; then
    rm file_para/temp_param.txt
fi
cp file_para/version_payment_param.txt file_para/temp_param.txt
rm file_para/version_payment_param.txt
LINE_NUMBER=`wc -l "file_para/temp_param.txt"`
TEMP_CONTENT_FOR_LINE=($LINE_NUMBER)
LINE_NUMBER=${TEMP_CONTENT_FOR_LINE[0]}
echo "LINE_NUMBER=$LINE_NUMBER"
if [ -z "$LINE_NUMBER" ]; then
    echo "now the config line in .txt is empty"
fi
for (( i = 1; i <= LINE_NUMBER; i++));do
    LINE_CONTENT=`sed -n ${i}p "file_para/temp_param.txt"`
    if [ $FLAG = 1 ]; then
        FLAG=0
        continue
    fi

    if [[ ${LINE_CONTENT:0:1} = ";" || $LINE_CONTENT = "" ]];then
        continue
    fi
    
    RESET_PROJECT
    
    PARAM=($LINE_CONTENT)
    VERSION_NAME=${PARAM[0]}
    for (( j = 1; j < ${#PARAM[@]}; j++ ));do
        echo ${PARAM[j]}
        case ${PARAM[j]} in
        -p) PACKAGE_NAME=${PARAM[j+1]};;
        -i) ICON_NAME=${PARAM[j+1]};;
        -m) IMAGES_NAME=${PARAM[j+1]};;
        -a) APP_NAME=${PARAM[j+1]};;
        -s) SMS_PROVIDER=${PARAM[j+1]};;
        -t) THIRD_PROVIDER=${PARAM[j+1]};;
        -f) SHOP_ITEM_FILTER=${PARAM[j+1]};;
        -c) CHANNEL_NAME=${PARAM[j+1]};;
        esac
    done
    echo VERSION_NAME=$VERSION_NAME
    echo PACKAGE_NAME=$PACKAGE_NAME
    echo ICON_NAME=$ICON_NAME
    echo IMAGES_NAME=$IMAGES_NAME
    echo APP_NAME=$APP_NAME
    echo SMS_PROVIDER=$SMS_PROVIDER
    echo THIRD_PROVIDER=$THIRD_PROVIDER
    echo SHOP_ITEM_FILTER=$SHOP_ITEM_FILTER
    echo CHANNEL_NAME=$CHANNEL_NAME
    
    echo "build version"
    cd $PROJECT_NAME/game_client
    if [ -d $VERSION_CONFIG_PATH ]; then
        echo "remove build_version"
        rm -r build_version
        mkdir build_version
    fi
    if [ -f $VERSION_CONFIG_PATH/build_version.sh ]; then
        echo "copy build_version.sh"
        cp $VERSION_CONFIG_PATH/build_version.sh build_version/
    else
        echo "please make sure build_version.sh uploaded correctly"
        exit 1
    fi
    
    if [ -d $VERSION_CONFIG_PATH/temp ]; then
        echo "copy config"
        cp -R $VERSION_CONFIG_PATH/temp/* build_version/
    else
        echo "$VERSION_CONFIG_PATH/temp not exit"
        exit 1
    fi

    cd $ROOT_PATH/$PROJECT_NAME/game_client/build_version
    chmod 777 build_version.sh
    VERSION_PARAM=`BUILD_VERSION_PARAM`
    TEMP_PATH=`pwd`
    ./build_version.sh $VERSION_PARAM
    cd $TEMP_PATH
    
    echo "build payment"
    cd $ORIGINAL_PATH
    cd $PROJECT_NAME/game_protocol_base/config_libpayment
    PAYMENT_PARAM=`BUILD_PAYMENT_PARAM`
    TEMP_PATH=`pwd`
    chmod 777 build_payment.sh
    ./build_payment.sh $PAYMENT_PARAM
    echo $PAYMENT_PARAM
    cd $TEMP_PATH
    
    #pack texture
    if [ $PACK_TEXTURE = true ]; then
        DO_PACK_TEXTURE
    fi

    echo "config server ip and port"
    
    echo "build release"
        currentPath=`pwd`
    cd $ROOT_PATH/$PROJECT_NAME/game_client/proj.android/assets
    if [ ! -f "game.properties" ]; then
        echo "game.properties not exist"
        exit 1
    fi    

    #config ip and port
    BUILD_RELEASE
    echo "finish build release"
    cd $currentPath
    #end config ip and port
    
    cd $TEMP_PATH
    ./build_payment.sh -s 0 -t 0 -g $VERSION_NAME
    
    cd $ORIGINAL_PATH
    
    VERSION_NAME=""
    PACKAGE_NAME=""
    ICON_NAME=""
    IMAGES_NAME=""
    APP_NAME=""
    SMS_PROVIDER=""
    THIRD_PROVIDER=""
done


set +e
