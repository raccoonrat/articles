import os
import time 

ORDERER = os.path.abspath("./deployment/ordererOrganizations") # it must point to the ordererOrgnaizations dir
PEER = os.path.abspath("./deployment/peerOrganizations") # it must point to the peerOrgnaizations dir

def deleteOrderers(path):
	orgs = os.listdir(path)
	for org in orgs:
		orgPath = os.path.join(path, org)
		namespaceYaml = os.path.join(orgPath, org + "-namespace.yaml" ) #orgYaml namespace.yaml
		
		for orderer in os.listdir(orgPath + "/orderers"):
			ordererPath = os.path.join(orgPath + "/orderers", orderer)
			ordererYaml = os.path.join(ordererPath, orderer + ".yaml")
			checkAndDelete(ordererYaml)

		time.sleep(5)
		checkAndDelete(namespaceYaml)




def deletePeers(path):
	orgs = os.listdir(path)
	for org in orgs:
		orgPath = os.path.join(path, org)

		namespaceYaml = os.path.join(orgPath, org + "-namespace.yaml" ) # namespace.yaml
		cliYaml = os.path.join(orgPath, org + "-cli.yaml" ) # cli.yaml  

		for peer in os.listdir(orgPath + "/peers"):
			peerPath = os.path.join(orgPath + "/peers", peer)
			peerYaml = os.path.join(peerPath, peer + ".yaml")
			checkAndDelete(peerYaml)	
		
		checkAndDelete(cliYaml) 

		time.sleep(5)               # keep namespace alive until every resources have been destroyed
		checkAndDelete(namespaceYaml)
		

def checkAndDelete(f):
	if os.path.isfile(f):
		os.system("kubectl delete -f " + f)

deleteOrderers(ORDERER)
deletePeers(PEER)
