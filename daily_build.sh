#!/bin/bash

#set true to clone and initialize this project
IS_INIT=true

#project name for git clone
PROJECT_NAME="game_majiang_client"

#abbreviation of game name
GAME_CODE="whmj"

#main branch
BRANCH_NAME="master"

#pack texture
PACK_TEXTURE=false

#proj.android path
ANDROID_PROJECT_PATH=src/game_majiang_client/proj.android

#proj.win32 path
WIN32_PROJECT_PATH=src/game_majiang_client/proj.win32

#build_pro.sh path
BUILD_CHANNEL_PATH="/home/buildbot/dev/android-channel"

#job root path
JOB_ROOT_PATH=`pwd`

echo "read environmental variable"
. /home/buildbot/.bashrc

#clone code and copy some file
INIT_PROJECT()
{
	local TEMP_PATH=`pwd`
	
	cd $JOB_ROOT_PATH
	rm -rf *
  
    git clone ssh://buildbot@game-git.baina.com:29418/$PROJECT_NAME
  
    cd $PROJECT_NAME
	git checkout $BRANCH_NAME

    git submodule init
    git submodule update
    cd ..
  
    echo "copy keystore"
    cp ../keystore/$GAME_CODE/* $PROJECT_NAME/$ANDROID_PROJECT_PATH
  
    echo "copy set_env_local.sh"
    cp ../local_variable/set_env_local.sh $PROJECT_NAME/$ANDROID_PROJECT_PATH
    chmod 777 $PROJECT_NAME/$ANDROID_PROJECT_PATH/set_env_local.sh

	echo "copy crashlytics.jar"
    if [ ! -d "$PROJECT_NAME/$ANDROID_PROJECT_PATH/libs/" ];then
        mkdir $PROJECT_NAME/$ANDROID_PROJECT_PATH/libs
    fi
    cp $PROJECT_NAME/game_client_framework/externals/crashlytics/crashlytics.jar \
       $PROJECT_NAME/$ANDROID_PROJECT_PATH/libs/

	echo "copy daily_build.sh"
	cp ../shell_for_version_build/daily_build.sh .

	cd $TEMP_PATH
}

#reset project
RESET_PROJECT()
{
	cd $JOB_ROOT_PATH
    cd $PROJECT_NAME
	
	echo "pull"
	git reset --hard HEAD
	git checkout $BRANCH_NAME
	git pull --rebase
	
	if [ $TAG_NAME != "default" ]; then
		git checkout $TAG_NAME
		if [ $? != 0 ]; then
			echo "!!! invalid tag !!!"
			exit 1
		fi
		echo "!!! checkout tag of $TAG_NAME"
	fi

    echo "update submodule"
    git submodule foreach git reset --hard HEAD
    git submodule update
	
}

#pack texture
DO_PACK_TEXTURE()
{
	local TEMP_PATH=`pwd`
	cd $JOB_ROOT_PATH
	cd $PROJECT_NAME
	cd $WIN32_PROJECT_PATH
	if [ -f rewrite.sh ] && [ -f package.sh ]; then
		rm -rf ../Resources/images/package
		chmod 777 rewrite.sh
		chmod 777 package.sh
		./rewrite.sh
	fi
	cd $TEMP_PATH
}

#init project
cd $JOB_ROOT_PATH
if [ $IS_INIT = true ]; then
    INIT_PROJECT
fi

RESET_PROJECT

if [ $PACK_TEXTURE = true ]; then
	DO_PACK_TEXTURE
fi

TEMP_PATH=`pwd`
cd $JOB_ROOT_PATH/$PROJECT_NAME/$ANDROID_PROJECT_PATH/assets
if [ "$TARGET_SERVER_ADDRESS" != "default" ]; then
	CURRENT_ADDRESS=`cat game.properties | grep "game.server="`
	sed -i -e "s/$CURRENT_ADDRESS/game.server=$TARGET_SERVER_ADDRESS/g" game.properties
	CURRENT_ADDRESS=`cat game_debug.properties | grep "game.server="`
	sed -i -e "s/$CURRENT_ADDRESS/game.server=$TARGET_SERVER_ADDRESS/g" game_debug.properties
fi
if [ "$TARGET_SERVER_PORT" != "default" ]; then
	CURRENT_ADDRESS=`cat game.properties | grep "game.port="`
	sed -i -e "s/$CURRENT_ADDRESS/game.port=$TARGET_SERVER_PORT/g" game.properties
	CURRENT_ADDRESS=`cat game_debug.properties | grep "game.port="`
	sed -i -e "s/$CURRENT_ADDRESS/game.port=$TARGET_SERVER_PORT/g" game_debug.properties
fi
cd ..
name_channelID=`cat AndroidManifest.xml | grep -A1 "TDGA_CHANNEL_ID" | grep "value" | cut -d "\"" -f 2`
if [ -n "$channelID" ]; then
    sed -i -e "s/\"${name_channelID}\"/\"${channelID}\"/g" AndroidManifest.xml
    line_channelID=`cat assets/ChannelId`
    sed -i -e "s/$line_channelID/$channelID/g" assets/ChannelId
fi
old_version_name=`cat AndroidManifest.xml | grep "android:versionName" | cut -d "\"" -f 2`
old_version_code=`cat AndroidManifest.xml | grep "android:versionCode" | cut -d "\"" -f 2`
if [ -n "$versionName" ]; then
    sed -i -e "s/\"${old_version_name}\"/\"${versionName}\"/g" AndroidManifest.xml
	versionName=`echo $versionName | sed "s/\.//g"`
    sed -i -e "s/\"${old_version_code}\"/\"${versionName}\"/g" AndroidManifest.xml
fi
cd $TEMP_PATH

cd $JOB_ROOT_PATH
chmod 777 daily_build.sh
./daily_build.sh $PROJECT_NAME
