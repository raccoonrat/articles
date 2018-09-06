import os
import yaml
from jinja2 import Template, Environment, FileSystemLoader

templateDirectory = os.path.abspath("./template")
configFilePath = os.path.abspath("./cluster-config.yaml")
cryptoConfigFile = os.path.abspath("./crypto-config.yaml")

clusterConfigTp = "cluster_config_template.yaml"

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

def generateCryptoYaml(env, OrdererOrgs, peerOrgs):
	OrdererOrg = OrdererOrganization(OrdererOrgs[0]["Name"], OrdererOrgs[0]["Domain"], OrdererOrgs[0]["Template"]["Count"], OrdererOrgs[0]["Type"])
	peerORGS = []
	for peerOrg in peerOrgs:
	    org = PeerOrganization(peerOrg["Name"], peerOrg["Domain"], peerOrg["Template"]["Count"])
	    peerORGS.append(org)

	cryptoConfig = open(cryptoConfigFile, "w")

	template = env.get_template(clusterConfigTp)
	content = template.render(ordererOrg=OrdererOrg, peerOrgs=peerORGS)
	cryptoConfig.write(content)

	cryptoConfig.close()

env = Environment(loader=FileSystemLoader(templateDirectory))
configYaml = loadYaml(configFilePath)
OrdererOrgs = configYaml["OrdererOrgs"]
peerOrgs = configYaml["PeerOrgs"]

generateCryptoYaml(env, OrdererOrgs, peerOrgs)
