#!/bin/bash

#unit:second
TIMEOUT=3600
UNIT=5

#start fabric
function startfabric () {
    cp resources /opt/share/ -rf 
    bash generateALL.sh
    python3.5 transform/run.py
}

#check peers status to see if they are running
function checkPeerStatus() {
    peerNum=`kubectl get pod --all-namespaces | grep peer | wc -l`
    runStatus="Running"
    ifready=0
    sum=`expr ${TIMEOUT} / ${UNIT}`
    for((m=0;m<${sum};m=m+1))
    do
       status=`kubectl get pod --all-namespaces | grep peer | awk  -F ' '  '{print $4}'`
       for((i=1;i<=${peerNum};i=i+1))
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
          echo "The BoV is up and running."
          break
       else
          sleep ${UNIT}
       fi
    done
    if [ $ifready == 0 ];then
        echo "Failed to pull Fabric images."
        exit 1
    fi   
}

function joinchannelAndUpdate () {
CREATE_CHANNEL="kubectl exec -it CLI_POD --namespace=CLI_NAMESPACE -- peer channel create -c defaultchannel -o ORDERER_ADDRESS -f ./resources/channel-artifacts/channel.tx"
CPOY_BLOCK="kubectl exec -it CLI_POD --namespace=CLI_NAMESPACE -- cp ./defaultchannel.block ./resources/channel-artifacts -rf" 
JOIN_CHANNEL="kubectl exec -it CLI_POD --namespace=CLI_NAMESPACE -- env CORE_PEER_ADDRESS=P_ADDRESS peer channel join -b ./resources/channel-artifacts/defaultchannel.block" 
UPDATE_CHANNEL="kubectl exec -it CLI_POD --namespace=CLI_NAMESPACE -- peer channel update -o ORDERER_ADDRESS -c defaultchannel -f ./resources/channel-artifacts/OrgORG_IDMSPanchors.tx"
CHANNEL_CREATED=0

ordererName=$(kubectl get pods --all-namespaces | grep orderer |  awk 'NR==2{print}'| awk '{print $2}' | sed 's/\-/\./')
ordererTmp=${ordererName%-*}
orderer=${ordererTmp%-*}

namespaces=$(kubectl get pods --all-namespaces | grep "cli-" | awk '{print $1}')
orgNum=1

for namespace in $namespaces
do
    peerNum=$(kubectl get pods --all-namespaces | grep ${namespace} | grep peer | wc -l)
    cli=$(kubectl get pods --all-namespaces | grep "cli-" | grep $namespace | awk '{print $2}')

    for((i=0;i<"$peerNum";i++));
    do
        if [ $CHANNEL_CREATED -eq 0 ];then
            # render commands 
            # CREATE_CHANNEL_C=$(echo $CREATE_CHANNEL | sed 's/CLI_POD/'$cli'/' | sed 's/CLI_NAMESPACE/'$namespace'/' | sed 's/ORDERER_ADDRESS/orderer0\.'${orderer%-*}'\:7050/')
            CREATE_CHANNEL_C=$(echo $CREATE_CHANNEL | sed 's/CLI_POD/'$cli'/' | sed 's/CLI_NAMESPACE/'$namespace'/' | sed  's/ORDERER_ADDRESS/'$orderer':7050/') 
            COPY_BLOCK_C=$(echo $CPOY_BLOCK | sed 's/CLI_POD/'$cli'/' | sed 's/CLI_NAMESPACE/'$namespace'/') 
            JOIN_CHANNEL_C=$(echo $JOIN_CHANNEL | sed 's/P_ADDRESS/peer'$i'\.'$namespace'\:7051/' |sed 's/CLI_POD/'$cli'/' | sed 's/CLI_NAMESPACE/'$namespace'/') 

            # execute commands
            # channel should be only created once.
            $CREATE_CHANNEL_C
            if [ $? != 0 ];then
                echo "Fail to create defaultchannel."
                exit 1
            fi
            sleep 2
            $COPY_BLOCK_C
            if [ $? != 0 ];then
                echo "Fail to config defaultchannel."
                exit 1
            fi
            sleep 2
            $JOIN_CHANNEL_C
            if [ $? != 0 ];then
                echo "peer$i fail to join defaultchannel on $namespace."
                exit 1
            fi
            sleep 1
            # change state
            CHANNEL_CREATED=1
        else
            # render channel join command
            JOIN_CHANNEL_C=$(echo $JOIN_CHANNEL | sed 's/P_ADDRESS/peer'$i'\.'$namespace'\:7051/' | sed 's/CLI_POD/'$cli'/' | sed 's/CLI_NAMESPACE/'$namespace'/') 
            # execute command
            $JOIN_CHANNEL_C
            if [ $? != 0 ];then
                echo "peer$i fail to join defaultchannel on $namespace."
                exit 1
            fi
            sleep 1
        fi
    done

    # update anchor peer
    # render channel update command
    UPDATE_CHANNEL_C=$(echo $UPDATE_CHANNEL | sed  's/ORDERER_ADDRESS/'$orderer':7050/' | sed 's/ORG_ID/'${orgNum}'/' | sed 's/CLI_POD/'$cli'/' | sed 's/CLI_NAMESPACE/'$namespace'/')
    # exec channel update command
    $UPDATE_CHANNEL_C 
    if [ $? != 0 ];then
       echo "peer$i fail to update defaultchannel on $namespace."
       exit 1
    fi
    orgNum=`expr $orgNum + 1`
done
}

function generateConfig (){
    python3.5 transform/explorerConfigGenerator.py
    mv config.json /opt/share/resources/explorer-artifacts
    hostname=`kubectl get node|grep master|awk {'print $1'}`
    ip=`kubectl describe node "$hostname"|grep ip`
    ip=${ip#*=}
    cp template/explorer_template.yaml /opt/share/resources/explorer-artifacts/fabric_1_0_explorer.yaml
    sed -i "s/SERVER_IP/'$ip'/g" /opt/share/resources/explorer-artifacts/fabric_1_0_explorer.yaml
}

function startFabricExplorer () {
    runStatus="Running"
    explorerNum=`kubectl get pod 2>&1 | grep fabric-explorer | wc -l`
    if [ ${explorerNum} == 0 ];then
       kubectl create -f /opt/share/resources/explorer-artifacts/fabric_1_0_explorer.yaml
    else
       echo "fabric-explorer is already installed."
       exit 1
    fi
    sleep ${UNIT}
    for((count=1;count<=300;count=count+1))
    do
        tmpStatus=`kubectl get pod 2>&1 | grep fabric-explorer | awk  -F ' '  '{print $3}'`
        if [ "${tmpStatus}" != "${runStatus}" ];then
           sleep ${UNIT}
        else
           break
        fi
    done
    if [ $count == 300 ];then
        echo "Failed to start up the fabric-explorer, Please refer the kubernetes logs for detail."
        exit 1
    fi
}

function showAddress () {
    #get fabric-explorer address
    explorerName=`kubectl get pods | grep -i fabric-explorer | awk  -F ' '  '{print $1}'`
    explorerIp=`kubectl describe pod ${explorerName} --namespace=default | grep Node | sed -n 1p | awk  -F '/'  '{print $2}'`
    explorerPort=`kubectl describe service fabric-explorer --namespace=default| grep -i NodePort |sed -n 2p | awk  -F ' '  '{print $3}' | awk  -F '/'  '{print $1}'`
    echo "You can view and monitor BoV at: http://${explorerIp}:${explorerPort}"
}


###########start install##########
startfabric
sleep ${UNIT}
checkPeerStatus
sleep ${UNIT}
joinchannelAndUpdate
sleep ${UNIT}
generateConfig
startFabricExplorer
sleep ${UNIT}
showAddress

