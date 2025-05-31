# psvrtool
CLI utility for controlling PSVR processor unit

Make sure to unbind usbhid driver from interface 5 before using this tool:

`sudo sh -c 'echo "x-x:1.5" > /sys/bus/usb/drivers/usbhid/unbind'`

# Usage
```
Usage: ./psvrtool [OPTIONS] SUBCOMMAND

Options:
  -h,--help                   Print this help message and exit

Subcommands:
  cinema_mode                 Render HDMI input as a flat 2D screen, with tracking and morphing handled by the processor unit
  poweroff                    Power off the PSVR headset and processor unit

```

## `cinema_mode` subcommand
```
Usage: ./psvrtool cinema_mode [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -b,--brightness INT:INT in [0 - 100]
                              Brightness of the screen (percentage, 0-100). Defaults to 35%
  -s,--screen-size INT:INT in [0 - 100]
                              Size of the screen (percentage, 0-100). Defaults to 35%

```
