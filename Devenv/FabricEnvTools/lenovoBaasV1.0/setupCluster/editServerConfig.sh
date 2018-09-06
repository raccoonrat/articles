mkdir /opt/share/resources/crypto-config/ordererOrganizations/ordererorg-f-1/fabric-ca-server
cp fabric-ca-server-config.yaml /opt/share/resources/crypto-config/ordererOrganizations/ordererorg-f-1/fabric-ca-server
orgs=$(dir /opt/share/resources/crypto-config/peerOrganizations)
orgs=$(echo $orgs | sed 's/ /,/g')
OLD_IFS="$IFS"
IFS=","
orgs=($orgs)
IFS="$OLD_IFS"

count=${#orgs[@]}
count=`expr $count - 1`

for i in $(seq 0 $count);
do
	mkdir /opt/share/resources/crypto-config/peerOrganizations/${orgs[$i]}/fabric-ca-server
	target="/opt/share/resources/crypto-config/peerOrganizations/${orgs[$i]}/fabric-ca-server"
	cp fabric-ca-server-config.yaml $target
done


