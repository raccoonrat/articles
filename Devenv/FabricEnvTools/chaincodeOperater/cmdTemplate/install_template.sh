#!/bin/sh

cd /opt/gopath/src
peer chaincode install -n {chaincode_name} -v 1.0 -p {chaincode_name} &> logs.txt
