#!/bin/sh

#
# functions.sh
# Copyright(c) 2013 Uli Fuchs <ufuchs@gmx.com>
# MIT Licensed
#

TRUE=0
FALSE=1

#   ---------------------------------------------
#   get verbose
#   ---------------------------------------------
header () {

    echo
    echo "    System initialization..."
    echo
    echo "    Hostname       : $HOSTNAME"
    echo "    Filesystem     : v$VERSION"
    echo
    echo "    Kernel release : `uname -s` `uname -r`"
    echo "    Kernel version : `uname -v`"
    echo
}

#   ---------------------------------------------
#   prints execution status.
#
#   @param {$1} : Execution status
#   @param {$2} : Optional
#          Continue (0) or Abort (1) on error
#   ---------------------------------------------
status () {

    [ $1 -eq 0 ] && {
            echo    "  [ OK ]"
        } || {
            echo    "  [ ERR ]"
           [ ${2:0} -eq 1 ] && {
               echo "[ ABORT ]"
               exit 1
           }
        }
}

#   ---------------------------------------------
#   check for a working network connection
#
#   @return : TRUE, if ping doesn't fail
#   ---------------------------------------------
isNetworkAvailable () {
    ping -w 2 -c 1 "8.8.8.8" >/dev/null 2>&1
    return $?
}

#   ---------------------------------------------
#   mounts an net file system
#   ---------------------------------------------
mountNFS () {
    echo -n "  * Mount NFS share"
    mount -t nfs -o nolock $NFS_SERVER:$NFS_EXPORT /nfs
    status $? 0
}

#   ---------------------------------------------
#   sets the clock in the kernel
#   ---------------------------------------------
start_ntpd () {
    echo -n "  * Adjust clock"
    ntpd -nq -p 0.europe.pool.ntp.org >/dev/null 2>&1
    status $? 0
    echo -n "    "
    date
}

#   ---------------------------------------------
#   starts telnetd
#   ---------------------------------------------
start_telnetd () {
    echo -n "  * Starting telnetd"
    telnetd -l /bin/sh
    status $? 0
}

#   ---------------------------------------------
#   starts crond
#   ---------------------------------------------
start_crond () {
    echo -n "  * Starting crond"
    crond -c /etc/cron.d/crontabs
    status $? 0
}

#   ---------------------------------------------
#   starts your applications
#   ---------------------------------------------
start_apps () {

    fetching_the_first_weather

}

#   ---------------------------------------------
#   fetches the weather images from server
#   ---------------------------------------------
fetching_the_first_weather () {
    echo "  * fetching weather"
    /usr/local/bin/fetchWeather.sh
}

#   ---------------------------------------------
#   starts the network related services
#   ---------------------------------------------
startUsbNetworkServices () {
    start_ntpd
    start_telnetd
    start_crond
}

#   ---------------------------------------------
#   starts the 'bnep' network related services
#   ---------------------------------------------
startBluezNetworkServices () {
    start_ntpd
    start_crond
}

#   ---------------------------------------------
#   gets the interface addr
#
#   @param {$1} : e.g 'usb0' or 'bnep0'
#   ---------------------------------------------
interfaceAddr () {
    echo -n $(echo `ip addr show $1 | tail -n 1` | cut -d' ' -f2)
}

#   ---------------------------------------------
#   inits the 'usb0' device
#   ---------------------------------------------
usb0_init () {

    local perDHCP=0

    echo -n "* Network usb0 "

    ifconfig usb0 up

    [ $perDHCP -eq 0 ] && {
        ifconfig usb0 172.16.1.2 netmask 255.255.255.0
        route add default gw 172.16.1.1 dev usb0
    } || {
        udhcpc -i usb0 -b -T 1 > /dev/null 2>&1
    }

    local res=$?

    interfaceAddr "usb0"

    status $res 0

}

#   ---------------------------------------------
#   inits the 'bnep0' device
#   ---------------------------------------------
bnep0_init () {

    echo -n "* Network bnep0 "

    ifconfig bnep0 up
    udhcpc -i bnep0 -b -T 1 > /dev/null 2>&1

    interfaceAddr "bnep0"

    status $? 0
}

#   ---------------------------------------------
#   inits the 'hci0' device
#   ---------------------------------------------
bluez_init () {

    echo "* Init Bluetooth"

    hciattach -s 115200 /dev/ttySAC0 bcm2035 921600 flow 00:00:00:00:00 >/dev/null 2>&1

    hciconfig hci0 up
    bdaddr $BT_ADDRESS >/dev/null 2>&1
    hciconfig hci0 reset
    hciconfig hci0 up
    hciconfig hci0 class 0x020100
    hciconfig hci0 name $HOSTNAME

}

#   ---------------------------------------------
#   connects the 'hci0' device
#   ---------------------------------------------
bluez_connect () {

}

#   ---------------------------------------------
#   scans for remote devices
#   ---------------------------------------------
bluez_scan () {

    REMOTE_DEVICES=$(hcitool hci0 scan)

    echo $REMOTE_DEVICES

}

#   ---------------------------------------------
#   connects the 'hci0' device
#   ---------------------------------------------
bluez_pair () {

}

