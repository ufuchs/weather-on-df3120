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
WEATHER_OBSERVER_LOCATION="1"


TMP_DIR="/tmp"
WEATHER_FILE_DIR="$TMP_DIR/weather"
WEATHER_FILE_NAME_BASE="weather"

#
# Fetchs a single weather file for a given `forecast day`
# @param {$1} forecastday
#
fetchSingleWeatherfile () {

    local forecastDay="$1"

    local weatherFilename="$TMP_DIR/$WEATHER_FILE_NAME_BASE"-$forecastDay.png

    local WGET_ARGS="-O $weatherFilename $URL/?forecastDay=$forecastDay"

    wget $WGET_ARGS # >/dev/null 2>&1

}

#
#
#
init () {

    URL="http://$WEATHER_SERVER/$WEATHER_OBSERVER_LOCATION"

    mkdir -p "$WEATHER_FILE_DIR" > /dev/null

}

init

for i in 0 1 2 3; do
    fetchSingleWeatherfile $i
done
