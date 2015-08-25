#!/bin/bash

for ((i=0; i<30; ++i))
do
	KGI_PLG_RUN=`ps aux | grep kgi_plugin_fix_svr | grep -v grep | wc -l`
	if [[ $KGI_PLG_RUN < 1 ]] ; then
		echo "`date +%F\ %T:` kgi plugin server is not running. Try to start..."
		/data/kgi/plugin_server/admin/start_kgi_plugin.sh
	fi

	sleep 2
done	
