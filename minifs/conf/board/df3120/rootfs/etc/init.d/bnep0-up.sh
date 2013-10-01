#!/bin/hush

ifconfig bnep0 up
udhcpc -i bnep0 --background -T 1  > /dev/null 2>&1
