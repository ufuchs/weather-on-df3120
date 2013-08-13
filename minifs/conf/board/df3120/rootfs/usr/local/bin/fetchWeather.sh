#!/bin/sh

#
# fetchWeather
# Copyright(c) 2013 Uli Fuchs <ufuchs@gmx.com>
# MIT Licensed
#
# [ In the end, it's not the years in your life that count. ]
# [ It's the life in your years.        - Abraham Lincoln - ]
#

#
# Your service url
#
WEATHER_SERVER="sleepy-fox-1911.herokuapp.com/weather/df3120"
WEATHER_OBSERVER_LOCATION="2"

URL="http://$WEATHER_SERVER/$WEATHER_OBSERVER_LOCATION"

WEATHER_DIR="/tmp"
WEATHER_FNAME_BASE="weather"

#
#
#
status () {


    echo -ne '\033[36G'

    [ $1 -eq 0 ] && {
            echo -ne '\033[0;32m'
            echo '[OK]'
        } || {
            echo -ne '\033[0;31m'
            echo -ne '\033[36G''[KO]'
            echo
            sleep 10
           [ ${2:0} -eq 1 ] && {
               echo "[ ABORT ]"
               exit 1
           }
        }
    echo -ne '\033[0;37m'
}

#
# Fetchs a single weather file for a given `forecast day`
# @param {$1} forecastday
#
fetchSingleWeatherfile () {

    local forecastDay="$1"
    local weatherFilename="$WEATHER_DIR/$WEATHER_FNAME_BASE"-$forecastDay.png
    local WGET_ARGS="-O $weatherFilename $URL/?forecastDay=$forecastDay"

    wget $WGET_ARGS >/dev/null 2>&1

    echo $?

}

#
#
#
fetchWeather () {

    echo "* Fetching weather"

    local res=

    local weatherFilename=""

    for i in 0 1 2 3; do

        weatherFilename="$WEATHER_FNAME_BASE"-$i.png

        touch /tmp/wip

        echo -n "  $weatherFilename "
        echo -ne "\033[s"               # save cursor's current position

        /usr/local/bin/progress.sh &

        res=$(fetchSingleWeatherfile $i)

        rm /tmp/wip

#        sleep 1

        echo -ne "\033[u"       # restore the cursor to the previous position
        echo -ne '\033[K'       # clear to end of line

        status $res 0

    done

}

fetchWeather
