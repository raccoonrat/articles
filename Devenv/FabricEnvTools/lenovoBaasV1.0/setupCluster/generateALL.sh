#!/bin/bash +x

TIMEOUT=3600
UNIT=5
CHANNEL_NAME=$1
: ${CHANNEL_NAME:="defaultchannel"}

export TOOLS=$PWD/../bin
export CONFIG_PATH=$PWD
export FABRIC_CFG_PATH=/opt/share/resources

function generateCerts (){
	CRYPTOGEN=$TOOLS/cryptogen
	$CRYPTOGEN generate --config=./crypto-config.yaml
	#rm crypto-config.yaml
}

function checkCAStatus() {
    caNum=`kubectl get pod --all-namespaces | grep ca | wc -l`
    runStatus="Running"
    ifready=0
    sum=`expr ${TIMEOUT} / ${UNIT}`
    for((m=0;m<${sum};m=m+1))
    do
       status=`kubectl get pod --all-namespaces | grep ca | awk  -F ' '  '{print $4}'`
       for((i=1;i<=${caNum};i=i+1))
       do
          tmpStatus=`echo ${status} | cut -d' ' -f${i}`
          if [ "${tmpStatus}" != "${runStatus}" ];then
             ifready=0
             break
          else
             ifready=1
          fi
       done
       if [ $ifready == 1 ];then
          echo "ca pods are ready."
          break
       else
          sleep ${UNIT}
       fi
    done
    if [ $ifready == 0 ];then
        exit 1
    fi
}

function replaceCerts (){
	chmod -R 664 ./crypto-config
        mv ./crypto-config /opt/share/resources/
        bash editServerConfig.sh
        python3.5 transform/runCA.py
        #bash tmp2.sh
        checkCAStatus
	echo "wait 1min here..."
	sleep 60
        bash replaceCerts.sh

}

function generateChannelArtifacts() {
	mkdir /opt/share/resources/channel-artifacts
	mv ./configtx.yaml /opt/share/resources
	CONFIGTXGEN=$TOOLS/configtxgen
	ORGS=$(cat /opt/share/resources/configtx.yaml | grep  -a -oP "(?<=[\&])Org[1-9]*$")
	cd /opt/share/resources
	$CONFIGTXGEN -profile OrdererGenesis -outputBlock /opt/share/resources/channel-artifacts/genesis.block
	$CONFIGTXGEN -profile DefaultChannel -outputCreateChannelTx /opt/share/resources/channel-artifacts/channel.tx -channelID $CHANNEL_NAME
	for i in $ORGS; do
	$CONFIGTXGEN -profile DefaultChannel -outputAnchorPeersUpdate /opt/share/resources/channel-artifacts/"$i"MSPanchors.tx -channelID $CHANNEL_NAME -asOrg "$i"MSP
	done
}

function relocate() {
	chmod -R 664 /opt/share/resources/channel-artifacts
	cp /opt/share/resources/channel-artifacts/genesis.block /opt/share/resources/crypto-config/ordererOrganizations/*
	mv /opt/share/resources/configtx.yaml /opt/share/resources/channel-artifacts
}

python3.5 transform/certConfigGenerator.py
generateCerts
python3.5 transform/configGenerator.py
replaceCerts
generateChannelArtifacts
relocate
