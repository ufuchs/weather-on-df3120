#!/bin/hush

# BE AWARE about the device in /etc/bluetooth/rfcomm.conf 
DEVICE=/dev/rfcomm0

echo "* Starting rfcomm..."
/usr/bin/rfcomm -f /etc/bluetooth/rfcomm.conf
#/usr/bin/rfcomm bind "$DEVICE"
/usr/bin/rfcomm watch "$DEVICE" 

