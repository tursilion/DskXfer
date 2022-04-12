20171024

TI99Recv is a simple program to transfer disk images from the TI to the PC over a standard serial cable.

The raw program file, DSKXFER, is the TI side. It has an XMODEM header on it, so use XModem and a terminal program to transfer it to the TI. Use E/A#5 to start DSKXFER.

The .DSK file is a TI DSK image that includes the DSKXFER program. Note the TI Source Code is intended to be built with c99 by Clint Pulley.

On the PC, start TI99Recv. By default it puts the files in C:\NEW\TI, but since you may not have that folder, click the browse button to select a destination folder.

This is a really simple program. It runs at 19.2k, and just sends one sector at a time. It expects the disk in DSK1, and uses the RS232 device through direct hardware access. On the PC, it expects to use COM1.

The TI side has two modes:

1) Bitmap - sends only the sectors on the floppy marked as used (usually faster)

2) Full disk - sends all sectors, used or not.

You can abort the TI side by holding '~' (FCTN-W). You can quit the TI side by pressing QUIT. Neither action will harm your floppy - it's only reading.

The PC side is a standard Windows app.

Source is available for both sides. Please contact me before redistributing it or modified versions of it.

