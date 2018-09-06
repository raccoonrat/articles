#!/bin/bash

python3.5 transform/delete.py
kubectl delete -f /opt/share/resources/explorer-artifacts/fabric_1_0_explorer.yaml
rm -r deployment/ /opt/share/resources
rm fabric-ca-server-config.yaml
rm crypto-config.yaml
