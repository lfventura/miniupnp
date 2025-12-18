#!/bin/bash
# VyOS port forwarding and filtering script to be ran by lfventura/miniupnp/miniupnpd
#
# Operations:
#   add_redirect <ifname> <rhost> <eport> <iaddr> <iport> <proto> <desc> <timestamp>
#   add_filter <ifname> <rhost> <iaddr> <eport> <iport> <proto> <desc>
#   delete_redirect <ifname> <eport> <proto>
#   delete_filter <ifname> <eport> <proto>
#   delete_redirect_and_filter <eport> <proto>

set -e

echo "0:$0 1:$1 2:$2 3:$3 4:$4 5:$5 6:$6 7:$7 8:$8 9:$9" >> /var/log/upnp_debug.log
echo "0:$0 1:$1 2:$2 3:$3 4:$4 5:$5 6:$6 7:$7 8:$8 9:$9"

rule_number() {
    local eport="$1"
    local proto="$2"

    # Gera o hash e pega os 5 primeiros caracteres hexadecimais.
    local hex_part
    hex_part=$(echo "$eport$proto" | md5sum | cut -c 1-5)

    # Converte o hexadecimal (base 16) para um número inteiro (base 10).
    #    A expansão do shell (prefixo 16#) faz a conversão.
    local decimal_num
    decimal_num=$((16#$hex_part))

    # Mapeia para a faixa segura: Usa módulo para garantir que o número gerado
    #    fique em um espaço grande (ex: 37000) e adiciona o offset 50000.
    #    O número final estará entre 50000 e 87000.
    local rule_num
    rule_num=$((decimal_num % 37000 + 50000))

    echo "$rule_num"
}

log() {
   local message="$1"
   echo "$EXEC_TIME - $message" >> /var/log/upnp_script.log
}

# GLOBAL
EXEC_TIME=$(date +"%Y-%m-%d %H:%M")
OPERATION="$1"
FIREWALL_NAME="WAN_TO_LAN_V4"

echo "$EXEC_TIME - $OPERATION $INT_IP -> $EXT_PORT:$INT_PORT" >> /var/log/upnp_script_summary.log

# Main dispatcher
case "$OPERATION" in
    add_redirect)
        echo "*************************** 1"
        IF_NAME="$2"
        RHOST="$3"
        EXT_PORT="$4"
        INT_IP="$5"
        INT_PORT="$6"
        PROTOCOL="${7,,}"
        DESC="$8"
        RULE_NUM=$(rule_number "$EXT_PORT" "$PROTOCOL")
        log "Adding redirect: $PROTOCOL $EXT_PORT -> $INT_IP:$INT_PORT (rhost: $RHOST, desc: $DESC)"
        sg vyattacfg -c "/adm_srv/vyos_add_redirect.sh '$RULE_NUM' '$IF_NAME' '$EXT_PORT' '$INT_IP' '$INT_PORT' '$PROTOCOL' '$DESC'"
        ;;
    add_filter)
        echo "*************************** 2"
        echo "FILTER PARAMS -> 0:$0 1:$1 2:$2 3:$3 4:$4 5:$5 6:$6 7:$7 8:$8 9:$9"
        RHOST="$3"
        INT_IP="$4"
        EXT_PORT="$5"
        INT_PORT="$6"
        PROTOCOL="${7,,}"
        DESC="$8"
        RULE_NUM=$(rule_number "$EXT_PORT" "$PROTOCOL")
        echo "RHOST:$RHOST INT_IP:$INT_IP EXT_PORT:$EXT_PORT INT_PORT:$INT_PORT PROTOCOL:$PROTOCOL DESC:$DESC RULE_NUM:$RULE_NUM"
        log "Adding filter: $PROTOCOL -> $INT_IP:$INT_PORT (rhost: $RHOST, desc: $DESC)"
        sg vyattacfg -c "/adm_srv/vyos_add_filter.sh '$FIREWALL_NAME' '$RULE_NUM' '$EXT_PORT' '$INT_IP' '$INT_PORT' '$PROTOCOL' '$DESC'"
        ;;
    delete_redirect)
        echo "*************************** 3"
        EXT_PORT="$2"
        PROTOCOL="${3,,}"
        RULE_NUM=$(rule_number "$EXT_PORT" "$PROTOCOL")
        log "Deleting redirect: $PROTOCOL $EXT_PORT"
        sg vyattacfg -c "/adm_srv/vyos_delete_redirect.sh '$RULE_NUM'"
        ;;
    delete_filter)
        echo "*************************** 4"
        EXT_PORT="$2"
        PROTOCOL="${3,,}"
        RULE_NUM=$(rule_number "$EXT_PORT" "$PROTOCOL")
        log "Deleting filter: $PROTOCOL $EXT_PORT"
        sg vyattacfg -c "/adm_srv/vyos_delete_filter.sh '$RULE_NUM'"

        ;;
    delete_redirect_and_filter)
        echo "*************************** 5"
        EXT_PORT="$2"
        PROTOCOL="${3,,}"
        RULE_NUM=$(rule_number "$EXT_PORT" "$PROTOCOL")
        log "Deleting redirect and filter: $PROTOCOL $EXT_PORT"
        sg vyattacfg -c "/adm_srv/vyos_delete_redirect.sh '$RULE_NUM'"
        sg vyattacfg -c "/adm_srv/vyos_delete_filter.sh '$FIREWALL_NAME' '$RULE_NUM'"
        ;;
    *)
        log "Unknown operation: $OPERATION"
        echo "*************************** 6"
        exit 1
        ;;
esac

exit 0