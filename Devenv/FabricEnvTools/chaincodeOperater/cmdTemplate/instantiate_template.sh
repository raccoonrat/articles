#!/bin/sh

cd /opt/gopath/src
peer chaincode instantiate -n {chaincode_name} -v 1.0 -C {channel_name} -c '{chaincode_args}' -P "{chaincode_policy}" &>> logs.txt
