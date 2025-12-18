#!/bin/vbash
FIREWALL_NAME="$1"
RULE_NUM="$2"
EXT_PORT="$3"
INT_IP="$4"
INT_PORT="$5"
PROTOCOL="$6"
DESC="$7"

echo "INNER FILTER -> 0:$0 1:$1 2:$2 3:$3 4:$4 5:$5 6:$6 7:$7 8:$8 9:$9"

source /opt/vyatta/etc/functions/script-template
configure

set firewall ipv4 name $FIREWALL_NAME rule $RULE_NUM action "accept"
set firewall ipv4 name $FIREWALL_NAME rule $RULE_NUM protocol "$PROTOCOL"
set firewall ipv4 name $FIREWALL_NAME rule $RULE_NUM destination address "$INT_IP"
set firewall ipv4 name $FIREWALL_NAME rule $RULE_NUM destination port "$INT_PORT"
set firewall ipv4 name $FIREWALL_NAME rule $RULE_NUM description "UPNP:$DESC:$INT_IP $EXT_PORT:$INT_PORT - $PROTOCOL"

commit
exit