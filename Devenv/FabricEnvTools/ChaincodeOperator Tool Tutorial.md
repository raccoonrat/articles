# ChaincodeOperator Tool Tutorial #
**Author: maxuyang**<br>
**Email: 18565814439@163.com**<br>
说明：此工具目前支持install&instantiate, query, invoke操作

## Prepare ##

1. 在master节点中，`ssh-keygen -t rsa`生成公钥，密码留空

2. 将生成的`/root/.ssh/id_rsa.pub`文件拷贝到每个node节点的`/root/.ssh/authorized_keys`目录

## Install&Instantiate chaincode ##

1. 将chaincode代码放到该工具目录下，修改chaincodeConfig文件：<br>
orgs name: /\* chaincode安装的org名单 \*/<br>
chaincode directory name: /\* chaincode代码文件夹的路径 \*/<br>
channel name: /\* chaincode安装的目标channel名字 \*/<br>
chaincode args: /\* chaincode初始化参数 \*/<br>
chaincode policy: /\* chaincode初始化policy \*/<br> 
示例：<br>
`orgs name:org1-f-1,org2-f-1`<br>
`chaincode directory name(if you are not installing, should be chaincode name):cc_ex`<br>
`channel name:defaultchannel`<br>
`chaincode args:{"Args":["init","john","100","peter","200"]}`<br>
`chaincode policy:OR ('Org1MSP.member','Org2MSP.member')`

2. `bash chaincodeOperator.sh install`<br>
注意：安装的chaincode名称与chaincode代码文件的名称一致

## Query chaincode ##

1. 修改chaincodeConfig文件：<br>
orgs name: /\* 可执行query操作的org \*/<br>
chaincode directory name: /\* chaincode名称 \*/<br>
channel name: /\* channel名字 \*/<br>
chaincode args: /\* 参数 \*/<br>
chaincode policy: /\* 留空 \*/<br> 
示例：<br>
`orgs name:org2-f-1`<br>
`chaincode directory name(if you are not installing, should be chaincode name):cc_ex`<br>
`channel name:defaultchannel`<br>
`chaincode args:{"Args":["query","john"]}`<br>
`chaincode policy:`

2. `bash chaincodeOperator.sh query`<br>

## Invoke chaincode ##

1. 修改chaincodeConfig文件：<br>
orgs name: /\* 可执行invoke操作的org \*/<br>
chaincode directory name: /\* chaincode名称 \*/<br>
channel name: /\* channel名字 \*/<br>
chaincode args: /\* 参数 \*/<br>
chaincode policy: /\* 留空 \*/<br> 
示例：<br>
`orgs name:org2-f-1`<br>
`chaincode directory name(if you are not installing, should be chaincode name):cc_ex`<br>
`channel name:defaultchannel`<br>
`chaincode args:{"Args":["invoke","john","peter","50"]}`<br>
`chaincode policy:`

2. `bash chaincodeOperator.sh invoke`