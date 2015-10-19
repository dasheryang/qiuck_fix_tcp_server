#!/bin/bash

TAR_DIR="/data/kgi/plugin_server/log/store_`date +%F_%H_%I_%s`"

#echo $TAR_DIR
SRC_DIR="/data/kgi/plugin_server/store"
#echo $TAR_DIR
#echo $SRC_DIR
echo "`date +%F\ %T` archving store file..."
mv $SRC_DIR $TAR_DIR
