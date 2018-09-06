#!/bin/sh

cd /opt/gopath/src
peer chaincode invoke -C {channel_name} -n {chaincode_name} -c '{chaincode_args}' &> logs.txt
