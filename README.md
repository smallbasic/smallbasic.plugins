## SmallBASIC plugins

Collection of loadable plugin modules to extend the functionality provided by SmallBASIC.

Install:
sudo apt install libmysqlclient-dev libglfw3-dev libglfw3 libglew-dev libtool

## Gallery

https://github.com/Immediate-Mode-UI/Nuklear.git

![screenshot_2018-02-28_21-09-09](https://user-images.githubusercontent.com/785121/36784602-bc6e1c32-1ccb-11e8-801b-a88c51a7b43a.png)

## Building under linux:

```
$ git clone https://github.com/smallbasic/smallbasic.plugins.git
$ cd smallbasic.plugins/
$ nano raylib/Makefile.am

Edit this line to specify the full path to sbasic:

sbasic = ...SmallBASIC/src/platform/console/sbasic

$ sh autogen.sh
$ ./configure --with-static-include
$ make -s -j 32

```

## How to run the nuklear demos

Setup the `SBASICPATH` environment variable to reference the compiled modules

`SBASICPATH=/home/chrisws/src/smallbasic.plugins/nuklear/.libs:src/modules/example/.libs:/home/chrisws/src/smallbasic.plugins/units`

Then execute the samples via the console version of SmallBASIC

```
$ cd smallbasic.plugins/nuklear
$ ~/src/SmallBASIC/src/platform/console/sbasic samples/calculator.bas
```

## Cross-compiling for windows

```
$ sudo apt install mingw-w64 gcc-mingw-w64
$ ./configure --host=x86_64-w64-mingw32
```

Required windows support DLLs:

- libgcc_s_seh-1.dll
- libstdc++-6.dll
- libwinpthread-1.dll

These can be obtained here: `https://sourceforge.net/projects/mingw-w64/files/`

Click the link: `x86_64-posix-seh`, then extract the files from the downloaded package.
