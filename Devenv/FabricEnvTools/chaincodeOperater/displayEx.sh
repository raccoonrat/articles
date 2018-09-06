#!/bin/sh

if [ "$1" == "install" ];then
	cp ex_install_config chaincodeConfig
elif [ "$1" == "query" ];then
	cp ex_query_config chaincodeConfig
elif [ "$1" == "invoke" ];then
	cp ex_invoke_config chaincodeConfig
fi
