# psvrtool
CLI utility for controlling PSVR processor unit

Make sure to unbind usbhid driver from interface 5 before using this tool:

`sudo sh -c 'echo "x-x:1.5" > /sys/bus/usb/drivers/usbhid/unbind'`