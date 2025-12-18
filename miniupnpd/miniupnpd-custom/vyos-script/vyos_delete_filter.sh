#!/bin/vbash
FIREWALL_NAME="$1"
RULE_NUM="$2"

echo "INNER DELFILTER -> $1"
echo "***********************************VOURODAR HEIN::::: delete firewall ipv4 name $FIREWALL_NAME rule $RULE_NUM"

source /opt/vyatta/etc/functions/script-template
configure
delete firewall ipv4 name $FIREWALL_NAME rule $RULE_NUM
commit
exit