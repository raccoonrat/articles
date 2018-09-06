import os
import yaml
from jinja2 import Template, Environment, FileSystemLoader

templateDirectory = os.path.abspath("./template")
k8sYamlDirectory = os.path.abspath("./deployment")
configFilePath = os.path.abspath("./cluster-config.yaml")
configtxFile = os.path.abspath("./configtx.yaml")
caServerConfigFile = os.path.abspath("./fabric-ca-server-config.yaml")

configtxTp = "configtx_template.yaml"
namespaceTp = "namespace_template.yaml"
ordererTp = "orderer_template.yaml"
cliTp = "cli_template.yaml"
peerTp = "peer_template.yaml"
caTp = "ca_template.yaml"
caServerConfigTp = "ca_server_config_template.yaml"

class PeerOrganization:
   def __init__(self, name, domain, count):
       self.name = name
       self.domain = domain
       self.count = count

class OrdererOrganization:
   def __init__(self, name, domain, count, orderer_type):
       self.name = name
       self.domain = domain
       self.count = count
       self.type = orderer_type

class KafkaBroker:
	def __init__(self, ipWithPort):
		self.ipWithPort = ipWithPort

def loadYaml(path):
    file = open(path, "r")
    Yaml = yaml.load(file)
    file.close()
    return Yaml

def generateConfigtxYaml(env, OrdererOrgs, peerOrgs, kafka):
	OrdererOrg = OrdererOrganization(OrdererOrgs[0]["Name"], OrdererOrgs[0]["Domain"], OrdererOrgs[0]["Template"]["Count"], OrdererOrgs[0]["Type"])
	peerORGS = []
	for peerOrg in peerOrgs:
	    org = PeerOrganization(peerOrg["Name"], peerOrg["Domain"], peerOrg["Template"]["Count"])
	    peerORGS.append(org)
	kafkaIPs = []
	for k in kafka:
		broker = KafkaBroker(k["IP"])
		if broker.ipWithPort!=None:
		   kafkaIPs.append(broker)

	configtx = open(configtxFile, "w")

	template = env.get_template(configtxTp)
	content = template.render(ordererOrg=OrdererOrg, peerOrgs=peerORGS, KafkaBrokers=kafkaIPs)
	configtx.write(content)

	configtx.close()
	
	caServerConfig = open(caServerConfigFile, "w")
	
	template = env.get_template(caServerConfigTp)
	content = template.render(ordererOrg=OrdererOrg, peerOrgs=peerORGS)
	caServerConfig.write(content)

	caServerConfig.close()

def generateK8sYaml(env, nfsServerIP, OrdererOrgs, peerOrgs, kafka):
	os.makedirs(k8sYamlDirectory)
	os.makedirs(k8sYamlDirectory + "/ordererOrganizations")
	os.makedirs(k8sYamlDirectory + "/peerOrganizations")
	os.makedirs(k8sYamlDirectory + "/ordererOrganizations/" + OrdererOrgs[0]["Domain"])
	os.makedirs(k8sYamlDirectory + "/ordererOrganizations/" + OrdererOrgs[0]["Domain"] + "/orderers")
	os.makedirs(k8sYamlDirectory + "/ordererOrganizations/" + OrdererOrgs[0]["Domain"] + "/orderers/orderer0." + OrdererOrgs[0]["Domain"])
	f = open(k8sYamlDirectory + "/ordererOrganizations/" + OrdererOrgs[0]["Domain"] + "/" + OrdererOrgs[0]["Domain"] + "-namespace.yaml", "w")
	template = env.get_template(namespaceTp)
	content = template.render(org=OrdererOrgs[0]["Domain"], \
				pvName=OrdererOrgs[0]["Domain"]+"-pv", \
				path="/opt/share/resources/crypto-config/ordererOrganizations/"+OrdererOrgs[0]["Domain"], \
				nfsServerIP=nfsServerIP)
	f.write(content)
	f.close()
	nodePortNum=30000
	dirs=os.listdir("./crypto-config/ordererOrganizations/"+OrdererOrgs[0]["Domain"]+"/ca")
	for d in dirs:
		if d.endswith("pem"):
			tlsCertFile = d
		else:
			tlsKeyFile = d
	f = open(k8sYamlDirectory + "/ordererOrganizations/" + OrdererOrgs[0]["Domain"] + "/" + OrdererOrgs[0]["Domain"] + "-ca.yaml", "w")
	template = env.get_template(caTp)
	content = template.render(namespace=OrdererOrgs[0]["Domain"], \
				tlsCert="/etc/hyperledger/fabric-ca-server-config/"+tlsCertFile, \
				tlsKey="/etc/hyperledger/fabric-ca-server-config/"+tlsKeyFile, \
				#command="fabric-ca-server init -b admin:adminpw", \
				command=" fabric-ca-server start --config /etc/hyperledger/fabric-ca-server/fabric-ca-server-config.yaml --ca.certfile /etc/hyperledger/fabric-ca-server-config/"+tlsCertFile+" --ca.keyfile /etc/hyperledger/fabric-ca-server-config/"+tlsKeyFile+" -b admin:adminpw -d ", \
				caPath="ca/", \
				pvName=OrdererOrgs[0]["Domain"]+"-pv", \
				nodePort=str(nodePortNum))
	f.write(content)
	f.close()
	f = open(k8sYamlDirectory + "/ordererOrganizations/" + OrdererOrgs[0]["Domain"] + "/orderers/orderer0." + OrdererOrgs[0]["Domain"] + "/orderer0." + OrdererOrgs[0]["Domain"] + ".yaml", "w")
	template = env.get_template(ordererTp)
	content = template.render(namespace=OrdererOrgs[0]["Domain"], \
				podName="orderer0-"+OrdererOrgs[0]["Domain"], \
				ordererID="orderer0", \
				localMSPID="OrdererMSP", \
				mspPath="orderers/orderer0."+OrdererOrgs[0]["Domain"]+"/msp", \
				tlsPath="orderers/orderer0."+OrdererOrgs[0]["Domain"]+"/tls", \
				proPath="orderers/orderer0."+OrdererOrgs[0]["Domain"]+"/production", \
				pvName=OrdererOrgs[0]["Domain"]+"-pv", \
				nodePort="32700")
	f.write(content)
	f.close()
	for peerOrg in peerOrgs:
		os.makedirs(k8sYamlDirectory + "/peerOrganizations/" + peerOrg["Domain"])
		os.makedirs(k8sYamlDirectory + "/peerOrganizations/" + peerOrg["Domain"] + "/peers")
		f = open(k8sYamlDirectory + "/peerOrganizations/" + peerOrg["Domain"] + "/" + peerOrg["Domain"] + "-namespace.yaml", "w")
		template = env.get_template(namespaceTp)
		content = template.render(org=peerOrg["Domain"], \
				pvName=peerOrg["Domain"]+"-pv", \
				path="/opt/share/resources/crypto-config/peerOrganizations/"+peerOrg["Domain"], \
				nfsServerIP=nfsServerIP)
		f.write(content)
		f.close()
		f = open(k8sYamlDirectory + "/peerOrganizations/" + peerOrg["Domain"] + "/" + peerOrg["Domain"] + "-cli.yaml", "w")
		template = env.get_template(cliTp)
		content = template.render(resourcesName=peerOrg["Domain"]+"-resources-pv", \
				nfsServerIP=nfsServerIP, \
				namespace=peerOrg["Domain"], \
				name="cli", \
				peerAddress="peer0."+peerOrg["Domain"]+":7051", \
				mspid=peerOrg["Name"]+"MSP", \
				mspPath="users/Admin@"+peerOrg["Domain"]+"/msp", \
				tlsPath="users/Admin@"+peerOrg["Domain"]+"/tls", \
				pvName=peerOrg["Domain"]+"-pv")
		f.write(content)
		f.close()
		nodePortNum = nodePortNum + 100
		dirs=os.listdir("./crypto-config/peerOrganizations/"+peerOrg["Domain"]+"/ca")
		for d in dirs:
			if d.endswith("pem"):
				tlsCertFile = d
			else:
				tlsKeyFile = d
		f = open(k8sYamlDirectory + "/peerOrganizations/" + peerOrg["Domain"] + "/" + peerOrg["Domain"] + "-ca.yaml", "w")
		template = env.get_template(caTp)
		content = template.render(namespace=peerOrg["Domain"], \
				tlsCert="/etc/hyperledger/fabric-ca-server-config/"+tlsCertFile, \
				tlsKey="/etc/hyperledger/fabric-ca-server-config/"+tlsKeyFile, \
				#command="fabric-ca-server init -b admin:adminpw", \
				command=" fabric-ca-server start --config /etc/hyperledger/fabric-ca-server/fabric-ca-server-config.yaml --ca.certfile /etc/hyperledger/fabric-ca-server-config/"+tlsCertFile+" --ca.keyfile /etc/hyperledger/fabric-ca-server-config/"+tlsKeyFile+" -b admin:adminpw -d ", \
				caPath="ca/", \
				pvName=peerOrg["Domain"]+"-pv", \
				nodePort=str(nodePortNum))
		f.write(content)
		f.close()
		for index in range(0, peerOrg["Template"]["Count"]):
			os.makedirs(k8sYamlDirectory + "/peerOrganizations/" + peerOrg["Domain"] + "/peers/peer" + str(index) + "." + peerOrg["Domain"])
			f = open(k8sYamlDirectory + "/peerOrganizations/" + peerOrg["Domain"] + "/peers/peer" + str(index) + "." + peerOrg["Domain"] + "/peer" + str(index) + "." + peerOrg["Domain"] + ".yaml", "w")
			template = env.get_template(peerTp)
			content = template.render(namespace=peerOrg["Domain"], \
				podName="peer"+str(index)+"-"+peerOrg["Domain"], \
				peerID="peer"+str(index), \
				corePeerID="peer"+str(index)+"."+peerOrg["Domain"], \
				peerAddress="peer"+str(index)+"."+peerOrg["Domain"]+":7051", \
				localMSPID=peerOrg["Name"]+"MSP", \
				peerGossip="peer"+str(index)+"."+peerOrg["Domain"]+":7051", \
				mspPath="peers/peer"+str(index)+"."+peerOrg["Domain"]+"/msp", \
				tlsPath="peers/peer"+str(index)+"."+peerOrg["Domain"]+"/tls", \
				proPath="peers/peer"+str(index)+"."+peerOrg["Domain"]+"/production", \
				pvName=peerOrg["Domain"]+"-pv")
			f.write(content)
			f.close()

env = Environment(loader=FileSystemLoader(templateDirectory))
configYaml = loadYaml(configFilePath)
OrdererOrgs = configYaml["OrdererOrgs"]
peerOrgs = configYaml["PeerOrgs"]
kafka = configYaml["Kafka"]

f=os.popen('bash transform/getIP.sh')
ip=f.read()
f.close()
print(ip)

generateConfigtxYaml(env, OrdererOrgs, peerOrgs, kafka)
generateK8sYaml(env, ip, OrdererOrgs, peerOrgs, kafka)
