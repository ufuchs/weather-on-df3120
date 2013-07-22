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
#   starts the network related services
#   ---------------------------------------------
startUsbNetworkServices () {

    echo -n "  * Adjust clock"
    ntpd -q -N -d -p 0.europe.pool.ntp.org
    sleep 1
    status $? 0

    echo -n "  * Starting telnetd..."
    telnetd -l /bin/sh
    status $? 0

    crond -c /etc/cron.d/crontabs

    echo "  * fetching weather"
    /usr/local/bin/fetchWeather.sh

}

#   ---------------------------------------------
#   starts the 'bnep' network related services
#   ---------------------------------------------
startBluezNetworkServices () {

    echo -n "  * Adjust clock"
    ntpd -q -p 0.europe.pool.ntp.org
    sleep 1
    status $? 0

    crond -c /etc/cron.d/crontabs

    echo "  * fetching weather"
    /usr/local/bin/fetchWeather.sh

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

