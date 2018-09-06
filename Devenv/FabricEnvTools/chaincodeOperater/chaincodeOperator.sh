#!/bin/sh

orgs=$(awk 'NR==1' chaincodeConfig)
chaincode_file=$(awk 'NR==2' chaincodeConfig)
channel_name=$(awk 'NR==3' chaincodeConfig)
chaincode_args=$(awk 'NR==4' chaincodeConfig)
chaincode_policy=$(awk 'NR==5' chaincodeConfig)

orgs=${orgs#*:}
OLD_IFS="$IFS"
IFS=","
orgs_array=($orgs) 
IFS="$OLD_IFS"

chaincode_file=${chaincode_file#*:}
channel_name=${channel_name#*:}
chaincode_args=${chaincode_args#*:}
chaincode_policy=${chaincode_policy#*:}

chaincode_name=${chaincode_file}

selfip=$(ip addr | grep 'state UP' -A2 | grep inet | awk '{print $2}' | awk 'NR==1{print}')
selfip=${selfip%/*}

if [ "$1" == "install" ];then

  for org in "${orgs_array[@]}"
  do
      pod_name=$(kubectl get pod --namespace="$org" | grep cli | awk '{print $1}')
      container_id=$(kubectl describe pod "$pod_name" --namespace="$org" |  grep "Container ID")
      container_id=${container_id#*//}
      ip=$(kubectl describe pod "$pod_name" --namespace="$org" |  grep "Node:")
      ip=${ip#*/}
      echo "########## install chaincode at $org ##########"
      cp cmdTemplate/install_template.sh installCC.sh
      sed -i "s/{chaincode_name}/${chaincode_name}/g" installCC.sh
      if [ "${selfip}" != "${ip}" ];then
          scp -r "${chaincode_file}" root@"${ip}":/root
          scp installCC.sh root@"${ip}":/root
      fi
      bash ssh.sh "${ip}" "${container_id}" "${chaincode_file}" installCC.sh
      rm installCC.sh   
  done
  
  echo "########## instatiate chaincode at $org ##########"
  cp cmdTemplate/instantiate_template.sh instantiateCC.sh
  sed -i "s/{chaincode_name}/${chaincode_name}/g;s/{channel_name}/${channel_name}/g;s/{chaincode_args}/${chaincode_args}/g;s/{chaincode_policy}/${chaincode_policy}/g" instantiateCC.sh
  if [ "${selfip}" != "${ip}" ];then
      scp instantiateCC.sh root@${ip}:/root
  fi
  bash ssh.sh "${ip}" "${container_id}" "${chaincode_file}" instantiateCC.sh
  rm instantiateCC.sh

elif [ "$1" == "query" ];then
  
  pod_name=$(kubectl get pod --namespace="$orgs" | grep cli | awk '{print $1}')
  container_id=$(kubectl describe pod "$pod_name" --namespace="$orgs" |  grep "Container ID")
  container_id=${container_id#*//}
  ip=$(kubectl describe pod "$pod_name" --namespace="$orgs" |  grep "Node:")
  ip=${ip#*/}

  echo "########## query chaincode at $orgs ##########"
  cp cmdTemplate/query_template.sh queryCC.sh
  sed -i "s/{chaincode_name}/${chaincode_name}/g;s/{channel_name}/${channel_name}/g;s/{chaincode_args}/${chaincode_args}/g" queryCC.sh
  if [ "${selfip}" != "${ip}" ];then
     scp queryCC.sh root@"${ip}":/root
  fi
  bash ssh.sh "${ip}" "${container_id}" "${chaincode_file}" queryCC.sh
  rm queryCC.sh

elif [ "$1" == "invoke" ];then

  pod_name=$(kubectl get pod --namespace="$orgs" | grep cli | awk '{print $1}')
  container_id=$(kubectl describe pod "$pod_name" --namespace="$orgs" |  grep "Container ID")
  container_id=${container_id#*//}
  ip=$(kubectl describe pod "$pod_name" --namespace="$orgs" |  grep "Node:")
  ip=${ip#*/}

  echo "########## invoke chaincode at $orgs ##########"
  cp cmdTemplate/invoke_template.sh invokeCC.sh
  sed -i "s/{chaincode_name}/${chaincode_name}/g;s/{channel_name}/${channel_name}/g;s/{chaincode_args}/${chaincode_args}/g" invokeCC.sh 
  if [ "${selfip}" != "${ip}" ];then
     scp invokeCC.sh root@"${ip}":/root
  fi
  bash ssh.sh "${ip}" "${container_id}" "${chaincode_file}" invokeCC.sh
  rm invokeCC.sh  

fi

