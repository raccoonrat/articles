#!/bin/bash 

orgs=$(kubectl get pod --all-namespaces | grep Terminating | awk '{print $1}')
orgs=$(echo $orgs | sed 's/ /,/g')
podnames=$(kubectl get pod --all-namespaces | grep Terminating | awk '{print $2}')
podnames=$(echo $podnames | sed 's/ /,/g')
OLD_IFS="$IFS" 
IFS="," 
orgs=($orgs) 
podnames=($podnames)
IFS="$OLD_IFS" 

count=${#orgs[@]}
count=`expr $count - 1`

for i in $(seq 0 $count);
do
	kubectl delete pod ${podnames[$i]} --namespace=${orgs[$i]} --grace-period=0 --force
done
