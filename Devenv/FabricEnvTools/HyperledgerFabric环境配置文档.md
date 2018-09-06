# Hyperledger Fabric环境配置文档 #
**Author: maxuyang**<br>
**Update: yunhao.wang**<br>
**Email: 18565814439@163.com**<br>

## 一、k8s环境配置 ##

依赖包：<br>
docker-ce\_17.03.0\_ce-0\_ubuntu-xenial\_amd64.deb<br>
images.tar.gz<br>
k8s\_debian.tar.gz

### 有用的环境配置和命令

BASH： <br>
source <(kubectl completion bash) # setup autocomplete in bash into the current shell, bash-completion package should be installed first.
echo "source <(kubectl completion bash)" >> ~/.bashrc # add autocomplete permanently to your bash shell.

kubectl config view # Show Merged kubeconfig settings.

kubectl explain pods,svc                       # get the documentation for pod and svc manifests
# Get commands with basic output
kubectl get services                          # List all services in the namespace
kubectl get pods --all-namespaces             # List all pods in all namespaces
kubectl get pods -o wide                      # List all pods in the namespace, with more details
kubectl get deployment my-dep                 # List a particular deployment
kubectl get pods --include-uninitialized      # List all pods in the namespace, including uninitialized ones

# Describe commands with verbose output
kubectl describe nodes my-node
kubectl describe pods my-pod

kubectl get services --sort-by=.metadata.name # List Services Sorted by Name

# List pods Sorted by Restart Count
kubectl get pods --sort-by='.status.containerStatuses[0].restartCount'

# Get the version label of all pods with label app=cassandra
kubectl get pods --selector=app=cassandra rc -o \
  jsonpath='{.items[*].metadata.labels.version}'

# Get all running pods in the namespace
kubectl get pods --field-selector=status.phase=Running

# Get ExternalIPs of all nodes
kubectl get nodes -o jsonpath='{.items[*].status.addresses[?(@.type=="ExternalIP")].address}'

# List Names of Pods that belong to Particular RC
# "jq" command useful for transformations that are too complex for jsonpath, it can be found at https://stedolan.github.io/jq/
sel=${$(kubectl get rc my-rc --output=json | jq -j '.spec.selector | to_entries | .[] | "\(.key)=\(.value),"')%?}
echo $(kubectl get pods --selector=$sel --output=jsonpath={.items..metadata.name})

# Check which nodes are ready
JSONPATH='{range .items[*]}{@.metadata.name}:{range @.status.conditions[*]}{@.type}={@.status};{end}{end}' \
 && kubectl get nodes -o jsonpath="$JSONPATH" | grep "Ready=True"

# List all Secrets currently in use by a pod
kubectl get pods -o json | jq '.items[].spec.containers[].env[]?.valueFrom.secretKeyRef.name' | grep -v null | sort | uniq

# List Events sorted by timestamp
kubectl get events --sort-by=.metadata.creationTimestamp


### master节点 ###

1. swapoff -a关闭swap分区

2. 安装docker，版本要求17.03ce

3. 解压images.tar.gz/k8s\_debian.tar.gz包

4. 进入解压出的images目录：<br>
    `docker load -i k8s-1-9-1.tar`<br>
   进入解压出的k8s\_debian目录：<br>
    `dpkg -i *.deb`

5. 初始化k8s集群，记录下最后一行用于后续node加入集群<br>
    `kubeadm init --kubernetes-version=v1.9.1 --pod-network-cidr=10.244.0.0/16 --apiserver-advertise-address=192.168.56.104`
    kubeadm init --kubernetes-version v1.9.1 --apiserver-advertise-address 192.168.56.104 --pod-network-cidr 10.244.0.0/16 --token 8c2350.f55343444a6ffc

6. 在.bashrc中添加：`export KUBECONFIG=/etc/kubernetes/admin.conf`

7. 到images目录下安装flannel插件<br>
    `kubectl apply -f kube-flannel.yml`

8. 默认情况下，为了保证master的安全，master是不会被调度到app的，通过输入可以取消这个限制：<br>
`kubectl taint nodes --all node-role.kubernetes.io/master-`

9. 查看所有pod状态：`kubectl get pod --all-namespaces`

### node节点 ###

1. 重复上述1~4步

2. 使用上述第5步记录的语句加入到集群当中<br>
示例：kubeadm join --token 1a9131.76881117845b498a 192.168.100.56:6443 --discovery-token-ca-cert-hash sha256:1a58c1abb1c408e0445a55d66b3cc3761c7cb10b7d88e725b3a1bd3c9032031a

### 拆除节点 ###

1. 在master节点上：<br>
`kubectl drain <node name> --delete-local-data --force --ignore-daemonsets`<br>
`kubectl delete node <node name>`

2. 在被拆除节点上：<br>
`kubeadm reset`

## 二、fabric环境配置 ##

依赖包：<br>
ccenv.tar<br>
dashboard.tar<br>
explorer.tar<br>
fabric1\_0\_5.tar<br>
mysql5\_7.tar<br>
baas-1.1.tar

###master节点###

1. 将上述前五个tar包全部load，baas-1.1.tar解压

2. 安装nfs server：`apt install nfs-kernel-server`

3. 在/etc/exports最后一行添加：<br>
`/opt/share *(rw,sync,no_subtree_check,no_root_squash)`

4. `mkdir -p /opt/share`

5. `service nfs-kernel-server restart`

6. 进入到baas目录，下载bin工具包：<br>
`curl https://nexus.hyperledger.org/content/repositories/releases/org/hyperledger/fabric/hyperledger-fabric/linux-amd64-1.0.5/hyperledger-fabric-linux-amd64-1.0.5.tar.gz | tar xz`

7. `chmod +x bin/*`

8. 进入setupCluster目录，修改cluster-config.yaml文件，定义你的组织结构 

9. 更改docker的仓库源：<br>
`vi /lib/systemd/system/docker.service`<br>
在ExecStart添加`--registry-mirror=http://f2d6cb40.m.daocloud.io`<br>
`systemctl daemon-reload`<br>
`systemctl restart docker`

###node节点###

1. 将上述前五个tar包全部load

2. `apt install nfs-common`

3. `mkdir -p /opt/share`

4. `mount -t nfs SERVER_IP:/opt/share /opt/share`
`mount -t nfs 192.168.1.105:/opt/share /opt/share`
`mount -t nfs 10.116.23.202:/opt/share /opt/share`


5. 更改docker的仓库源：<br>
`vi /lib/systemd/system/docker.service`<br>
在ExecStart添加`--registry-mirror=http://f2d6cb40.m.daocloud.io`<br>
`systemctl daemon-reload`<br>
`systemctl restart docker`

###启动步骤###

1. 在master节点上进入baas/setupCluster

2. `bash install.sh`启动

###停止步骤###

1. `bash stop.sh`

2. `rm -r deployment/ /opt/share/resources/` 