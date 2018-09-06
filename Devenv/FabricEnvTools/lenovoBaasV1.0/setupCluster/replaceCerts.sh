#!/bin/bash

ordererCAName=`kubectl get pod --all-namespaces | grep ca | grep order | awk '{print $2}'`
ordererNamespace=`kubectl get pods --all-namespaces | grep "ca-" | awk '{print $1}' | awk 'NR==1{print}'`
namespaces=`kubectl get pods --all-namespaces | grep "ca-" | awk '{if (NR>1){print $1}}'`

ENROLLADMIN="kubectl exec -it CAPOD --namespace=NAMESPACE -- fabric-ca-client enroll -u http://admin:adminpw@localhost:7054"
REGISTERNODE="kubectl exec -it CAPOD --namespace=NAMESPACE -- fabric-ca-client register --id.name IDNAME --id.type IDTYPE --id.affiliation ORG.department1 --id.secret IDSECRET"
ENROLLNODE="kubectl exec -it CAPOD --namespace=NAMESPACE -- fabric-ca-client enroll -u http://IDNAME:IDSECRET@localhost:7054 -M /etc/hyperledger/fabric-ca-server/MSP"

ENROLL_ORDERER_ADMIN=$(echo $ENROLLADMIN | sed 's/CAPOD/'$ordererCAName'/' | sed 's/NAMESPACE/'$ordererNamespace'/g' )
REGISTER_ORADMIN=$(echo $REGISTERNODE | sed 's/CAPOD/'$ordererCAName'/' | sed 's/NAMESPACE/'$ordererNamespace'/' | sed 's/IDNAME/Admin@'$ordererNamespace'/g' | sed 's/IDTYPE/ca/g' | sed 's/IDSECRET/orderer0adminpw/g' | sed 's/ORG/admin/g' )
ENROLL_ORADMIN=$(echo $ENROLLNODE | sed 's/CAPOD/'$ordererCAName'/' | sed 's/NAMESPACE/'$ordererNamespace'/' | sed 's/IDNAME/Admin@'$ordererNamespace'/g' | sed 's/IDSECRET/orderer0adminpw/g' | sed 's/MSP/adminMSP/g' )
REGISTER_ORDERER=$(echo $REGISTERNODE | sed 's/CAPOD/'$ordererCAName'/' | sed 's/NAMESPACE/'$ordererNamespace'/' | sed 's/IDNAME/orderer0.'$ordererNamespace'/g' | sed 's/IDTYPE/peer/g' | sed 's/IDSECRET/orderer0pw/g' | sed 's/ORG/dev/g' )
ENROLL_ORDERER=$(echo $ENROLLNODE | sed 's/CAPOD/'$ordererCAName'/' | sed 's/NAMESPACE/'$ordererNamespace'/' | sed 's/IDNAME/orderer0.'$ordererNamespace'/g' | sed 's/IDSECRET/orderer0pw/g' | sed 's/MSP/ordererMSP/g' )


rm /opt/share/resources/crypto-config/ordererOrganizations/$ordererNamespace/fabric-ca-server/msp/keystore/*
#echo $ENROLL_ORDERER_ADMIN
#echo $REGISTER_ORADMIN --id.attrs "hf.admin=true:ecert"
#echo $ENROLL_ORADMIN
#echo $REGISTER_ORDERER
#echo $ENROLL_ORDERER

$ENROLL_ORDERER_ADMIN
$REGISTER_ORADMIN --id.attrs "hf.admin=true:ecert"
$ENROLL_ORADMIN
$REGISTER_ORDERER
$ENROLL_ORDERER

cp /opt/share/resources/crypto-config/ordererOrganizations/$ordererNamespace/fabric-ca-server/adminMSP/signcerts/* /opt/share/resources/crypto-config/ordererOrganizations/$ordererNamespace/msp/admincerts/*
cp /opt/share/resources/crypto-config/ordererOrganizations/$ordererNamespace/fabric-ca-server/adminMSP/signcerts/* /opt/share/resources/crypto-config/ordererOrganizations/$ordererNamespace/users/Admin@$ordererNamespace/msp/admincerts/*
rm /opt/share/resources/crypto-config/ordererOrganizations/$ordererNamespace/users/Admin@$ordererNamespace/msp/keystore/*
cp /opt/share/resources/crypto-config/ordererOrganizations/$ordererNamespace/fabric-ca-server/adminMSP/keystore/* /opt/share/resources/crypto-config/ordererOrganizations/$ordererNamespace/users/Admin@$ordererNamespace/msp/keystore/
cp /opt/share/resources/crypto-config/ordererOrganizations/$ordererNamespace/fabric-ca-server/adminMSP/signcerts/* /opt/share/resources/crypto-config/ordererOrganizations/$ordererNamespace/users/Admin@$ordererNamespace/msp/signcerts/*
cp /opt/share/resources/crypto-config/ordererOrganizations/$ordererNamespace/fabric-ca-server/adminMSP/signcerts/* /opt/share/resources/crypto-config/ordererOrganizations/$ordererNamespace/orderers/orderer0.$ordererNamespace/msp/admincerts/*
rm /opt/share/resources/crypto-config/ordererOrganizations/$ordererNamespace/orderers/orderer0.$ordererNamespace/msp/keystore/*
cp /opt/share/resources/crypto-config/ordererOrganizations/$ordererNamespace/fabric-ca-server/ordererMSP/keystore/* /opt/share/resources/crypto-config/ordererOrganizations/$ordererNamespace/orderers/orderer0.$ordererNamespace/msp/keystore/
cp /opt/share/resources/crypto-config/ordererOrganizations/$ordererNamespace/fabric-ca-server/ordererMSP/signcerts/* /opt/share/resources/crypto-config/ordererOrganizations/$ordererNamespace/orderers/orderer0.$ordererNamespace/msp/signcerts/*

for namespace in $namespaces
do
	rm /opt/share/resources/crypto-config/peerOrganizations/$namespace/fabric-ca-server/msp/keystore/*
	org=${namespace%%-*}
	CA_Name=`kubectl get pods --all-namespaces | grep $namespace | awk '{print $2}'`
	ENROLL_PEER_ADMIN=$(echo $ENROLLADMIN | sed 's/CAPOD/'$CA_Name'/' | sed 's/NAMESPACE/'$namespace'/g' )
	REGISTER_ADMIN=$(echo $REGISTERNODE | sed 's/CAPOD/'$CA_Name'/' | sed 's/NAMESPACE/'$namespace'/' | sed 's/IDNAME/Admin@'$namespace'/g' | sed 's/IDTYPE/ca/g' | sed 's/IDSECRET/peeradminpw/g' | sed 's/ORG/admin/g' )
	ENROLL_ADMIN=$(echo $ENROLLNODE | sed 's/CAPOD/'$CA_Name'/' | sed 's/NAMESPACE/'$namespace'/' | sed 's/IDNAME/Admin@'$namespace'/g' | sed 's/IDSECRET/peeradminpw/g' | sed 's/MSP/adminMSP/g' )
	REGISTER_USER=$(echo $REGISTERNODE | sed 's/CAPOD/'$CA_Name'/' | sed 's/NAMESPACE/'$namespace'/' | sed 's/IDNAME/User1@'$namespace'/g' | sed 's/IDTYPE/user/g' | sed 's/IDSECRET/user1pw/g' | sed 's/ORG/user/g' )
	ENROLL_USER=$(echo $ENROLLNODE | sed 's/CAPOD/'$CA_Name'/' | sed 's/NAMESPACE/'$namespace'/' | sed 's/IDNAME/User1@'$namespace'/g' | sed 's/IDSECRET/user1pw/g' | sed 's/MSP/userMSP/g' )
	#echo $ENROLL_PEER_ADMIN
	#echo $REGISTER_ADMIN --id.attrs "hf.admin=true:ecert"
	#echo $ENROLL_ADMIN
	#echo $REGISTER_USER
	#echo $ENROLL_USER

	$ENROLL_PEER_ADMIN
        $REGISTER_ADMIN --id.attrs "hf.admin=true:ecert"
        $ENROLL_ADMIN
        $REGISTER_USER
        $ENROLL_USER

	path="deployment/peerOrganizations/$namespace/peers"
	array_name=`dir $path`

	cp /opt/share/resources/crypto-config/peerOrganizations/$namespace/fabric-ca-server/adminMSP/signcerts/* /opt/share/resources/crypto-config/peerOrganizations/$namespace/msp/admincerts/*
	cp /opt/share/resources/crypto-config/peerOrganizations/$namespace/fabric-ca-server/adminMSP/signcerts/* /opt/share/resources/crypto-config/peerOrganizations/$namespace/users/Admin@$namespace/msp/admincerts/*
	cp /opt/share/resources/crypto-config/peerOrganizations/$namespace/fabric-ca-server/adminMSP/signcerts/* /opt/share/resources/crypto-config/peerOrganizations/$namespace/users/Admin@$namespace/msp/signcerts/*
	rm /opt/share/resources/crypto-config/peerOrganizations/$namespace/users/Admin@$namespace/msp/keystore/*
	cp /opt/share/resources/crypto-config/peerOrganizations/$namespace/fabric-ca-server/adminMSP/keystore/* /opt/share/resources/crypto-config/peerOrganizations/$namespace/users/Admin@$namespace/msp/keystore/
	cp /opt/share/resources/crypto-config/peerOrganizations/$namespace/fabric-ca-server/userMSP/signcerts/* /opt/share/resources/crypto-config/peerOrganizations/$namespace/users/User1@$namespace/msp/admincerts/*
	cp /opt/share/resources/crypto-config/peerOrganizations/$namespace/fabric-ca-server/userMSP/signcerts/* /opt/share/resources/crypto-config/peerOrganizations/$namespace/users/User1@$namespace/msp/signcerts/*
	rm /opt/share/resources/crypto-config/peerOrganizations/$namespace/users/User1@$namespace/msp/keystore/*
	cp /opt/share/resources/crypto-config/peerOrganizations/$namespace/fabric-ca-server/userMSP/keystore/* /opt/share/resources/crypto-config/peerOrganizations/$namespace/users/User1@$namespace/msp/keystore/
	

	for peer in $array_name
	do
		peer=${peer%.*}
		pw=$peer"pw"
		dir=$peer"MSP"
		REGISTER_PEER=$(echo $REGISTERNODE | sed 's/CAPOD/'$CA_Name'/' | sed 's/NAMESPACE/'$namespace'/' | sed 's/IDNAME/'$peer'.'$namespace'/' | sed 's/IDTYPE/peer/g' | sed 's/IDSECRET/'$pw'/' | sed 's/ORG/dev/g' )
	        ENROLL_PEER=$(echo $ENROLLNODE | sed 's/CAPOD/'$CA_Name'/' | sed 's/NAMESPACE/'$namespace'/' | sed 's/IDNAME/'$peer'.'$namespace'/' | sed 's/IDSECRET/'$pw'/' | sed 's/MSP/'$dir'/' )
		#echo $REGISTER_PEER
		#echo $ENROLL_PEER
		$REGISTER_PEER
                $ENROLL_PEER
		cp /opt/share/resources/crypto-config/peerOrganizations/$namespace/fabric-ca-server/adminMSP/signcerts/* /opt/share/resources/crypto-config/peerOrganizations/$namespace/peers/$peer.$namespace/msp/admincerts/*
		cp /opt/share/resources/crypto-config/peerOrganizations/$namespace/fabric-ca-server/$dir/signcerts/* /opt/share/resources/crypto-config/peerOrganizations/$namespace/peers/$peer.$namespace/msp/signcerts/*
		rm /opt/share/resources/crypto-config/peerOrganizations/$namespace/peers/$peer.$namespace/msp/keystore/*
		cp /opt/share/resources/crypto-config/peerOrganizations/$namespace/fabric-ca-server/$dir/keystore/* /opt/share/resources/crypto-config/peerOrganizations/$namespace/peers/$peer.$namespace/msp/keystore/
		
	done
done


