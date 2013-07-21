#!/bin/sh

#
# functions.sh
# Copyright(c) 2013 Uli Fuchs <ufuchs@gmx.com>
# MIT Licensed
#

TRUE=0
FALSE=1

#   ---------------------------------------------
#   prints execution status.
#
#   @param {$1} : Execution status
#   @param {$2} : Optional
#          Continue (0) or Abort (1) on error
#   ---------------------------------------------
status () {

    [ $1 -eq 0 ] && {
            echo    "[SUCCESS]"
        } || {
            echo    "[FAILED]"
           [ ${2:0} -eq 1 ] && {
               echo "... System init aborted."
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
#   starts the network related services
#   ---------------------------------------------
startNetworkServices () {

    echo -n "* mount /nfs"
    mount -t nfs -o nolock $NFS_SERVER:$NFS_EXPORT /nfs
    status $? 0

    echo -n "* adjust clock"
    ntpd -q -p 0.europe.pool.ntp.org
    status $? 0

    echo -n "* Starting telnetd..."
    telnetd -l /bin/sh
    status $? 0

    crond -c /etc/cron.d/crontabs

    echo "* fetching weather"
    /usr/local/bin/fetchWeather.sh

}
