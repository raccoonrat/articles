import os

BASEDIR = os.path.dirname(__file__)
ORDERER = os.path.join(BASEDIR, "../deployment/ordererOrganizations") # it must point to the ordererOrgnaizations dir
PEER = os.path.join(BASEDIR, "../deployment/peerOrganizations") # it must point to the peerOrgnaizations dir

def runOrderers(path):
	orgs = os.listdir(path)
	for org in orgs:
		orgPath = os.path.join(path, org)

		for orderer in os.listdir(orgPath + "/orderers"):
			ordererPath = os.path.join(orgPath + "/orderers", orderer)
			ordererYaml = os.path.join(ordererPath, orderer + ".yaml")
			checkAndRun(ordererYaml)

def runPeers(path):
	orgs = os.listdir(path)
	for org in orgs:
		orgPath = os.path.join(path, org)
		
		cliYaml = os.path.join(orgPath, org + "-cli.yaml" ) # namespace.yaml
		checkAndRun(cliYaml)		

		for peer in os.listdir(orgPath + "/peers"):
			peerPath = os.path.join(orgPath + "/peers", peer)
			peerYaml = os.path.join(peerPath, peer + ".yaml")
			checkAndRun(peerYaml)


def checkAndRun(f):
	if os.path.isfile(f):
		os.system("kubectl create -f " + f)

	else:
		print("file %s no exited"%(f))
if __name__ == "__main__":
	runOrderers(ORDERER)
	runPeers(PEER)
