HiTechnic EV3 IRLink Blocks Readme 

Version 0.1

Revision History:
0.1    Initial release

This is a preliminary release of HiTechnic IRLink EV3 block for the new LEGO Mindstorms EV3.
    
This Action blocks makes it possible to use the IRLink sensor to communicate with the 
LEGO Power Functions (PF) Receiver with the LEGO EV3.  The block supports three 'modes'.

The first mode is known as the Combo Direct mode.  This mode sends the same type if 
IR messages as the LEGO 8885 remote.  It should be notes that these PF messages are
subject to a timeout on the LEGO PF receiver.  To keep the motors running, you need to
repeat the command at least every second or the motors will automatically stop.

The Second mode is known as the Single Output mode.  This mode only allows you to control
one of the two motors, A or B, connected to the PF remote.  This mode lets you select a 
speed from -7 (full reverse) to 7 full forword.  0 is float and 8 is brake.  Unlike the
Combo Direct, this mode is not subject to timeout and the motor will keep running 
until another command is received to change the motor speed.

The third mode supported by the block is the Send Nibble Data mode.  This mode takes
three nibbles (a value from 0 to 15) of data and sends them via the IRLink according
to the LEGO Power Functions protocol.  Please see the LEGO Power Functions RC
document to learn what modes are supported by the PF receiver. 

You can find a copy of the LEGO Power Functions Version 1.2 document here:
http://www.philohome.com/pf/LEGO_Power_Functions_RC_v120.pdf

Installation:
1) Extract the .ev3b file from the downloaded zip file.  
2) From the LEGO EV3 software, select Block Import from the Tools menu.
3) From Block Import, Browse to the folder where the .ev3b file is located.
4) Select the block file.
5) Click on Import.
6) Restart the EV3 software.


