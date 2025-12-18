#!/bin/vbash
RULE_NUM="$1"

echo "INNER DELNAT -> $1"

source /opt/vyatta/etc/functions/script-template
configure

delete nat destination rule $RULE_NUM

commit
exit