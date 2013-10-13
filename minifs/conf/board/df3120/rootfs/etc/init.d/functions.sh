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
#   @param {$1} Integer : Execution status,
#                         0 for [OK] or 1 for [ko]
#   @param {$2} Integer : Optional, continue (0)
#                         or abort (1) on error
#   ---------------------------------------------
status () {

    echo -ne '\033[36G'                # starts writing on position 36

    [ $1 -eq 0 ] && {
            echo -ne '\033[0;32m'      # sets text color back to green
            echo '[OK]'
        } || {
            echo -ne '\033[0;31m'      # sets text color back to red
            echo '[ko]'
            sleep 10
           [ ${2:0} -eq 1 ] && {
               echo "[ ABORT ]"
               exit 1
           }
        }

    echo -ne '\033[0;37m'              # sets text color back to white
}

#   ---------------------------------------------
#   writes a text `$1` with color `$2` and
#   attribute `$3`.
#
#   @param {$1} String : Your text
#   @param {$2} Integer : Your color from 30..37
#   @param {$3} Integer : Yout attribute,
#                         0 from normal,
#                         1 for bold/bright
#   ---------------------------------------------
writeAttributedText () {

    echo -ne '\033['$3';'$2'm'         # colorize text

    echo -n "$1"

}

#   ---------------------------------------------
#   writes a text `$1` with color `$2` and
#   attribute `$3` and, and sets the cursor
#   position back to the start so the text can
#   be overridden by an other function.
#
#   @param {$1} String : Your text
#   @param {$2} Integer : Your color from 30..37
#   @param {$3} Integer : Yout attribute,
#                         0 from normal,
#                         1 for bold/bright
#   ---------------------------------------------
writeAttributedTextWithFallback () {


    echo -ne '\033[s'                  # save cursor's current position

    writeAttributedText "$1" $2 $3

    echo -ne '\033[u'                  # restore the cursor to the previous position

}

#   ---------------------------------------------
#   check for a working network connection
#
#   @return : TRUE, if ping doesn't fail
#   ---------------------------------------------
isNetworkAvailable () {
    ping -w 2 -c 1 "8.8.8.8" >/dev/null 2>&1
    echo $?
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
    echo -n "  * Adjust clock "

    echo -ne "\033[s"                  # save cursor's current position

    touch /tmp/wip

    /usr/local/bin/progress.sh &       # starts progress the points .........

    ntpd -nq -p 0.europe.pool.ntp.org >/dev/null 2>&1
                                       # fetching by `ntpd` takes a while ...

    rm /tmp/wip                        # stops the progress points

    echo -ne "\033[u"                  # restore the cursor to the previous position
    echo -ne '\033[K'                  # clear to end of line

    status $? 0
    echo -n "    "
    date                               # writes the system date
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
    crond -c /etc/cron.d/crontabs > /dev/null 2>&1
    status $? 0
}

#   ---------------------------------------------
#   starts your applications
#   ---------------------------------------------
start_apps () {

    /usr/local/bin/fetchWeather.sh
    echo '* Waiting for 5 seconds...'
    sleep 5

    sdlweather &

}

#   ---------------------------------------------
#   starts the network related services
#   ---------------------------------------------
startUsbNetworkServices () {
    start_ntpd
    start_telnetd
#   start_crond
#   mountNFS
}

#   ---------------------------------------------
#   starts the 'bnep' network related services
#   ---------------------------------------------
startBluezNetworkServices () {
    start_ntpd
    start_crond
}

#   ---------------------------------------------
#   gets the network interface addr
#
#   @param {$1} String : e.g 'usb0' or 'bnep0'
#   ---------------------------------------------
get_InterfaceAddr () {
    echo -n $(echo `ip addr show $1 | tail -n 1` | cut -d' ' -f2)
}

#   ---------------------------------------------
#   inits the 'usb0' device
#   ---------------------------------------------
usb0_init () {

    local perDHCP=0

    echo -n "* Network usb0 "

    ifconfig usb0 down

    [ $perDHCP -eq 0 ] && {
        ifconfig usb0 172.16.1.1 netmask 255.255.255.0 up
        route add default gw 172.16.1.254 dev usb0
    } || {
        udhcpc -i usb0 -b -T 1 > /dev/null 2>&1
    }

    local res=$?

    get_InterfaceAddr "usb0"

    status $res 0

}

#   ---------------------------------------------
#   inits the 'hci0' device
#   @see : # http://bluetooth-pentest.narod.ru/software/bluetooth_class_of_device-service_generator.html
#   ---------------------------------------------
bluez_init () {

# http://www.linuxdevcenter.com/pub/a/linux/2006/09/21/rediscovering-bluetooth.html?page=3
# http://books.google.de/books?id=onLanBHwFooC&pg=PA198&lpg=PA198&dq=sdptool+add&source=bl&ots=Dp81uFRXgq&sig=Ihm7K9-tAIvkkTmbQRTKUofbGPM&hl=en&sa=X&ei=TyX-UdPFE8bVtAafj4GYBQ&redir_esc=y#v=onepage&q=sdptool%20add&f=false
# http://books.google.de/books?id=ta58VCBmLOkC&pg=PT260&lpg=PT260&dq=sdptool+add&source=bl&ots=30WQggQOsH&sig=VRhs5czASjKDmooZVmMbCaJ8568&hl=en&sa=X&ei=TyX-UdPFE8bVtAafj4GYBQ&redir_esc=y#v=onepage&q=sdptool%20add&f=false

    echo -n "* Init Bluez "

    mkdir -p /usr/var/run/dbus
    dbus-daemon --system

    ln -s /usr/lib/libdbus-1.so.3.5.2 /usr/lib/libdbus-1.so.3 > /dev/null 2>&1

    hciattach -s 115200 /dev/ttySAC0 bcm2035 921600 flow 03:44:36:4C:80:54 >/dev/null 2>&1

    hciconfig hci0 up
    bdaddr $BT_ADDRESS >/dev/null 2>&1
    hciconfig hci0 reset

    bluetoothd

    # keep the order of the following sequences
    # THE ARE SOME UNIDENTIFIED RACE CONDITIONS YET

    echo -n $(echo `hcitool dev | tail -n 1` | cut -d' ' -f2)

    /bin/sh -c 'agent 1234' > /dev/null &

    sleep 1

    status $res 0

    echo '  '$(echo `sdptool add PANU`)

    hciconfig hci0 auth

}

#   ---------------------------------------------
#   connects the 'hci0' device
#   ---------------------------------------------
bluez_connect () {

    echo -n "* Network bnep0 "

    pand --connect $BT_PARTNER --devup /etc/init.d/bnep0-up.sh

    local result=$?

    trap "echo Break..." SIGHUP SIGINT SIGTERM

    # save cursor's current position
    echo -ne "\033[s"

    # waiting for finishing 'bnep0-up.sh' esp. the DHCP client
    while [ -z "$(pand --list)" ] ; do echo -n '.'; sleep 1; done
    echo -n '!'

    # waiting for an ip on bnep0
    while [ -z "$( ifconfig bnep0 | grep 'inet addr' )" ] ; do echo -n '.'; sleep 1; done
    echo -n '!'

    # waiting for a valid route
    while [ -z "$( ping -w 1 -c 1 8.8.8.8 | grep 'ttl' )" ] ; do echo -n '.'; sleep 1; done
    echo -n '!'

    # restore the cursor to the previous position
    echo -ne "\033[u"
    # clear to end of line
    echo -ne '\033[K'

    get_InterfaceAddr "bnep0"

    status $result 0

}

#   ---------------------------------------------
#   scans for remote devices
#   ---------------------------------------------
bluez_scan () {

    REMOTE_DEVICES=$(hcitool hci0 scan)

    echo $REMOTE_DEVICES

}
