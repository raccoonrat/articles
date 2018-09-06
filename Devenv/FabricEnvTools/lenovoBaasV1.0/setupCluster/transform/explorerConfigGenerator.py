from jinja2 import Template, Environment, FileSystemLoader
import os, sys, yaml, copy, json
sys.path.append(os.path.dirname(__file__))

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


# ==== orderer relative ====
ORDERER = {
    "url": "grpc://orderer{}.{}:7050",
    "server-host": "orderer{}.{}",
}

# ==== peer relative =====
PEERORG = {
    "name": "peer{}",
    "mspid": "{}MSP",
    "ca": "http://ca.{}:7054"
}

ADMIN = {
    "key": "/first-network/crypto-config/peerOrganizations/{0}/users/Admin@{0}/msp/keystore",
    "cert": "/first-network/crypto-config/peerOrganizations/{0}/users/Admin@{0}/msp/signcerts",
}
PEER = {
    "requests": "grpc://peer{}.{}:7051",
    "events": "grpc://peer{}.{}:7053",
    "server-hostname": "peer{}.{}",
    "tls_cacerts": "/first-network/crypto-config/peerOrganizations/{1}/peers/peer{0}.{1}/tls/ca.crt"
}
# **** End of define ****


def dataPath(fileName):
    currentDir = os.path.dirname(__file__)
    parentDir = os.path.join(currentDir, "../")
    return os.path.join(parentDir, fileName)

def closeFiles(*openedFiles):
    for f in openedFiles:
        f.close()

def loadYaml(path):
    stream = open(path, "r")
    Yaml = yaml.load(stream)
    closeFiles(stream)
    return Yaml

def loadJson(path):
    jsonFile = open(path, "r")
    Json = json.load(jsonFile)
    closeFiles(jsonFile)
    return Json 

def jinjaEnv(templates_path):
    env = Environment(
        loader=FileSystemLoader(templates_path),
        trim_blocks=True,
        lstrip_blocks=True
    )
    return env

def handlerPeerNetwork(peerOrg):
    name = peerOrg["Name"]
    domain = peerOrg["Domain"]
    count = peerOrg["Template"]["Count"]

    # ----- peer org instance ------
    org = copy.copy(PEERORG)
    org["name"] = org["name"].format(domain)
    org["mspid"] = org["mspid"].format(name)
    org["ca"] = org["ca"].format(domain)

    # ----- peer instances -----
    for j in range(count):
        peer = copy.copy(PEER)
        peer["requests"] = peer["requests"].format(j, domain)
        peer["events"] = peer["events"].format(j, domain)
        peer["server-hostname"] = peer["server-hostname"].format(j, domain)
        peer["tls_cacerts"] = peer["tls_cacerts"].format(j, domain)
        org.update({"peer" + str(j): peer})
 
    # ----- admin instance ----
    admin = copy.copy(ADMIN)
    admin["key"] = admin["key"].format(domain)
    admin["cert"] = admin["cert"].format(domain)
    org.update({"admin": admin})

    return org

def handlerOrdererNetwork(ordererOrg):
    # ----------------- orderer organization ---------
    orderersList = []

    for i in range(ordererOrg["Template"]["Count"]):
       name = ordererOrg["Name"]
       domain = ordererOrg["Domain"]

       # ---------- orderer org instance -------
       orderer = copy.copy(ORDERER)
       orderer["url"] = orderer["url"].format(i, domain)
       orderer["server-host"] = orderer["server-host"].format(i, domain)
       orderersList.append(orderer)

    return orderersList 
 
def renderNetwork(Yaml, output, template):
   ordererOrg = Yaml['OrdererOrgs'][0]
   peerOrgs = Yaml['PeerOrgs']

   config = template
   network = {}

   orderersList = handlerOrdererNetwork(ordererOrg)
   network.update({"orderer": orderersList})
   
   # handle peers
   _count = 1
   for peerOrg in peerOrgs:
       org = handlerPeerNetwork(peerOrg)
       nickName = "org" + str(_count)
       _count += 1

       network.update({nickName: org})
       config["org"].append(nickName)

   # update template
   config.update({"network-config": network})

   # write to file config.json
   content = json.dumps(config, indent=4, sort_keys=True)
   networkConfig = open(output, 'w')
   networkConfig.write(content)
   closeFiles(networkConfig)
   

if __name__ == "__main__":
    cryptoConfig = dataPath("crypto-config.yaml")
    cryptoYaml = loadYaml(cryptoConfig)

    # generate network config based on crypto-config.yaml
    networkConfig = dataPath("config.json")
    networkTemplatePath = dataPath("resources/explorer-artifacts/config.json")
    networkTemplate = loadJson(networkTemplatePath)
    renderNetwork(cryptoYaml, networkConfig, networkTemplate)
    

