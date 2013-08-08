#!/bin/hush

interfaceAddr () {
    echo -n $(echo `ip addr show $1 | tail -n 1` | cut -d' ' -f2)
}


bnep0_init () {

    echo -n "* Network bnep0 "

    ifconfig bnep0 up

    [ $perDHCP -eq 0 ] && {
        ifconfig bnep0 172.16.1.2 netmask 255.255.255.0
        route add default gw 172.16.1.254 dev bnep
    } || {
        udhcpc -i bnep0 -b -T 1 > /dev/null 2>&1
    }

    interfaceAddr "bnep0"

}
