import os

BASEDIR = os.path.dirname(__file__)
ORDERER = os.path.join(BASEDIR, "../deployment/ordererOrganizations") # it must point to the ordererOrgnaizations dir
PEER = os.path.join(BASEDIR, "../deployment/peerOrganizations") # it must point to the peerOrgnaizations dir

def runOrdererCA(path):
	orgs = os.listdir(path)
	for org in orgs:
		orgPath = os.path.join(path, org)
		namespaceYaml = os.path.join(orgPath, org + "-namespace.yaml" ) #orgYaml namespace.yaml
		checkAndRun(namespaceYaml)

		caYaml = os.path.join(orgPath, org + "-ca.yaml" )
		checkAndRun(caYaml)

def runPeersCA(path):
	orgs = os.listdir(path)
	for org in orgs:
		orgPath = os.path.join(path, org)

		namespaceYaml = os.path.join(orgPath, org + "-namespace.yaml" ) # namespace.yaml
		checkAndRun(namespaceYaml)
		
		caYaml = os.path.join(orgPath, org + "-ca.yaml" ) # namespace.yaml
		checkAndRun(caYaml)		

def checkAndRun(f):
	if os.path.isfile(f):
		os.system("kubectl create -f " + f)

	else:
		print("file %s no exited"%(f))
if __name__ == "__main__":
	runOrdererCA(ORDERER)
	runPeersCA(PEER)
