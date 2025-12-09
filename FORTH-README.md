There is a port of mecrisp-stellaris
(https://mecrisp-stellaris-folkdoc.sourceforge.io/words.html#example-dictionary-listing)
for the CM4 core on the STM32H745. It can run independently on the M4 core
while smoothie runs on the M7 core.

This is a full blown forth kernel which can access most peripherals on the
board, and can also be used for scripting things like probing or canned
moves. It can do this by telling smoothie what it wants to do by queing gcode
commands like a shell would do.

To get started you need to copy the forth-cm4.bin found in the tools directory
to the sdcard, (you can do this in several ways the easiest being run MSC on
smoothie, mount the sdcard and copy the file then safely eject the sdcard).

Once copied to the sdcard you would need to edit the config.ini and add...

	[forth]
	enable = true

then reboot.

You then need to flash the forth kernel one time by typing `fth flash forth-cm4.bin` in a console.

Once the forth is flashed, you can access it by connecting to smoothie over a
USB port (it is recommended you enable the second USB serial port and use
that). Then you can start up a terminal session that talks diectly to the
forth kernel by using the command `fth terminal`.

This will then be talking to the forth kernel and not smoothie. You can return
to smoothie by typing control-D. As is normal with forth you can
interactively define new words and test them out from this terminal. You can
save words to flash using the compiletoflash word (see mecrisp stellaris
docs, link above). NOTE you will need to turn local echo off in your terminal
when talking to forth.(EG in picocom Control-A Control-C toggles local
echo).

For development I recommend using a front end to talk to forth like e4thcom
which has line editing, command history, and loading forth files.
Alternatively you can use picocom and lose the line editing but you can still
load forth file using the xfer.py utility found in the tools/forth directory.
launch script for picocom are also found there as well as a 64bit linux
binary of e4thcom. To do this you would connect to say /dev/ttyACM1 using
picocom and get into the forth terminal with the `fth t` command. then exit
picocom and run e4thcom to connect to /dev/ttyACM1.

