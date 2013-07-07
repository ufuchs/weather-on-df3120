#!/bin/sh

echo "* Fetching weather"

WEATHER_SERVER="xxxxxxxxxxxxxx.herokuapp.com/weather/df3120"
export WEATHER_SERVER

WEATHER_OBSERVER_LOCATION="1"
export WEATHER_OBSERVER_LOCATION

/usr/local/bin/fetchWeather.sh

