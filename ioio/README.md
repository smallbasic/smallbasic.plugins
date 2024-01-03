
https://github.com/ytai/ioio/wiki


https://github.com/ytai/ioio/blob/master/applications/HelloIOIOService/src/main/java/ioio/examples/hello_service/HelloIOIOService.java
https://github.com/ytai/ioio/wiki/IOIOLib-Core-API

/etc/udev/rules.d/50-ioio.rules

```
ACTION=="add", SUBSYSTEM=="tty", SUBSYSTEMS=="usb", ATTRS{idVendor}=="1b4f", ATTRS{idProduct}=="0008", SYMLINK+="IOIO%n", MODE="666"
ATTRS{idVendor}=="1b4f", ATTRS{idProduct}=="0008", ENV{ID_MM_DEVICE_IGNORE}="1"
```

looper:

  init
     AnalogInput in = ioio.openAnalogInput(pinNum);

  loop:


```
SB-> openAnalogInput(pin) -> JNI -> JAVA -> (pin)
     service.methods.push(function (ioio) { ioio.openAnalogInput(pin) });
     service.initMethods.push(function (ioio) { ioio.openAnalogInput(pin) });
     returns handle

     init:
        initmethods.pop().invoke();

     loop
        methods.pop().invoke()

```


When you plug an IOIO-OTG board into a USB port on your Linux machine, you can check which serial port has been assigned to the device by using various commands and tools. Here are a few methods:

Check dmesg Logs:

Open a terminal and run the following command to view the kernel logs:

bash
Copy code
dmesg | tail
Look for lines related to the connected USB device. The assigned serial port might be mentioned in the logs.

Use lsusb and udevadm:

Use lsusb to list the connected USB devices and note the device ID.

bash
Copy code
lsusb
Use udevadm to get detailed information about the device:

bash
Copy code
udevadm info -a -n /dev/ttyUSB0
Replace /dev/ttyUSB0 with the appropriate device file.

Check /dev/ Directory:

After connecting the device, run the following command to list the /dev/ directory:

bash
Copy code
ls /dev/ttyUSB*
This should show you the assigned serial port(s) for connected USB devices.

Use minicom or screen:

Install minicom if it's not already installed:

bash
Copy code
sudo apt-get install minicom
Run minicom:

bash
Copy code
minicom -D /dev/ttyUSB0
Replace /dev/ttyUSB0 with the actual serial port.

Alternatively, use screen:

bash
Copy code
screen /dev/ttyUSB0
Replace /dev/ttyUSB0 with the actual serial port.
