#!/bin/sh

while :; do

    [ ! -f /tmp/wip ] && {
        break
    }

    echo -n '.'

    sleep 1

done
