#!/bin/vbash
RULE_NUM="$1"
IF_NAME="$2"
EXT_PORT="$3"
INT_IP="$4"
INT_PORT="$5"
PROTOCOL="$6"
DESC="$7"

echo "INNER NAT -> 0:$0 1:$1 2:$2 3:$3 4:$4 5:$5 6:$6 7:$7 8:$8 9:$9"

source /opt/vyatta/etc/functions/script-template
configure

set nat destination rule $RULE_NUM description "UPNP:$DESC:$INT_IP $EXT_PORT:$INT_PORT - $PROTOCOL"
set nat destination rule $RULE_NUM destination port "$EXT_PORT"
set nat destination rule $RULE_NUM inbound-interface name "$IF_NAME"
set nat destination rule $RULE_NUM protocol "$PROTOCOL"
set nat destination rule $RULE_NUM translation address "$INT_IP"
set nat destination rule $RULE_NUM translation port "$INT_PORT"

commit
exit