#!/bin/sh

ip=$1
container_id=$2
chaincode_file=$3
exec_bash=$4
chaincode_name=${chaincode_file%.*}

selfip=$(ip addr | grep 'state UP' -A2 | grep inet | awk '{print $2}' | awk 'NR==1{print}')
selfip=${selfip%/*}

echo $exec_bash

#function
execFunc(){
  if [ "$1" != "$2" ];then
      ssh root@"$2" "cd /root && docker cp $3 $4:/opt/gopath/src/github.com/hyperledger/fabric/peer"
      ssh root@"$2" "docker exec -d $4 bash $3"
      sleep 3
      ssh root@"$2" "docker exec -d $4 rm $3"
      ssh root@"$2" "cd /root && rm $3"
      ssh root@"$2" "docker cp $4:/opt/gopath/src/logs.txt /root/"
      ssh root@"$2" "cd /root/ && cat logs.txt"
  else
      docker cp "$3" "$4":/opt/gopath/src/github.com/hyperledger/fabric/peer
      docker exec -d "$4" bash "$3"
      sleep 3
      docker exec -d "$4" rm "$3"
      docker cp $4:/opt/gopath/src/logs.txt /root/
      cd /root
      cat logs.txt
  fi
}

if [ "${exec_bash}" == "installCC.sh" ];then

  if [ "${selfip}" != "${ip}" ];then
      ssh root@"${ip}" "cd /root \
      && docker cp ${chaincode_file} ${container_id}:/opt/gopath/src/" 
      ssh root@"${ip}" "cd /root && rm -r ${chaincode_file}"
  else
      docker cp -r ${chaincode_file} ${container_id}:/opt/gopath/src/
  fi
  execFunc ${selfip} ${ip} ${exec_bash} ${container_id}

elif [ "${exec_bash}" == "instantiateCC.sh" -o "${exec_bash}" == "queryCC.sh" -o "${exec_bash}" == "invokeCC.sh" ];then
  
  execFunc ${selfip} ${ip} ${exec_bash} ${container_id}

fi

