## Links

https://github.com/ytai/ioio/wiki
https://github.com/ytai/ioio/blob/master/applications/HelloIOIOService/src/main/java/ioio/examples/hello_service/HelloIOIOService.java
https://github.com/ytai/ioio/wiki/IOIOLib-Core-API


## Setup

/etc/udev/rules.d/50-ioio.rules

```
ACTION=="add", SUBSYSTEM=="tty", SUBSYSTEMS=="usb", ATTRS{idVendor}=="1b4f", ATTRS{idProduct}=="0008", SYMLINK+="IOIO%n", MODE="666"
ATTRS{idVendor}=="1b4f", ATTRS{idProduct}=="0008", ENV{ID_MM_DEVICE_IGNORE}="1"
```

## Investigating connection issues

When you plug an IOIO-OTG board into a USB port on your Linux machine, you can check which serial port has been 
assigned to the device by using various commands and tools. Here are a few methods:

## Check dmesg Logs:

Open a terminal and run the following command to view the kernel logs:

```
dmesg | tail
```

Look for lines related to the connected USB device. The assigned serial port might be mentioned in the logs.

## Use lsusb

Use lsusb to list the connected USB devices and note the device ID.

```
lsusb
```

## Use udevadm to get detailed information about the device:

```
udevadm info -a -n /dev/ttyUSB0
```
Replace /dev/ttyUSB0 with the appropriate device file.

## Check /dev/ Directory:

After connecting the device, run the following command to list the /dev/ directory:

``
ls /dev/ttyUSB*
``

This should show you the assigned serial port(s) for connected USB devices.
