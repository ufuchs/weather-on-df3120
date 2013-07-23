#!/bin/bash
# vim: syntax=sh

#
# this script 
# 1.configures the usb0 interface
# 2.adds an entry in the routing table

INTERFACE=$1

# interface
ifconfig $INTERFACE down
ifconfig $INTERFACE 172.16.1.1 netmask 255.255.255.0 up

# routing
#route add -host 172.16.1.2 $INTERFACE

# firewall
iptables --table nat --append POSTROUTING --out-interface eth0 -j MASQUERADE
iptables --append FORWARD --in-interface $INTERFACE -j ACCEPT

echo 1 > /proc/sys/net/ipv4/ip_forward	

service isc-dhcp-server restart
