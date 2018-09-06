hostname=`kubectl get node|grep master|awk {'print $1'}`
ip=`kubectl describe node "$hostname"|grep ip`
ip=${ip#*=}
echo $ip
