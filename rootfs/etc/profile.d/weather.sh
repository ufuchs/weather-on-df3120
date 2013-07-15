#!/bin/sh

# 
# /etc/profile.d/weather.sh
# Copyright(c) 2013 Uli Fuchs <ufuchs@gmx.com>
# MIT Licensed
#

#
# 1.sets the environment variables
# 2.fetches the first weather images after system startup
#

echo "* Fetching weather"

WEATHER_SERVER="xxxxxxxxxxxxxx.herokuapp.com/weather/df3120"
export WEATHER_SERVER

WEATHER_OBSERVER_LOCATION="1"
export WEATHER_OBSERVER_LOCATION

/usr/local/bin/fetchWeather.sh

