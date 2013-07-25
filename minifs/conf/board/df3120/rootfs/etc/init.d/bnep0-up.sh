#!/bin/hush

ifconfig bnep0 192.168.1.1 netmask 255.255.255.0 up
route add default gw 192.168.1.2 bnep0

