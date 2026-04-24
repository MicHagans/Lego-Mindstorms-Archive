HiTechnic EV3 Analog Blocks Readme 

Version 0.1

Revision History:
0.1    Initial release of the three analog blocks

This is a preliminary release of HiTechnic sensor blocks for the new LEGO Mindstorms EV3.  The 
HiTechnic.ev3 file contains 5 HiTechnic sensor blocks:
    Gyro 	(re-released, now grouped with the other analog blocks)
    Force
    Magnetic
    
These sensor blocks make it possible to program with these sensors in the EV3 software.  Except for
the Angle sensor, only Measure modes are supported.  To use these blocks in a loop or with a switch, 
you will need to use your own Comparison block and then use the result of the comparison with the loop
or switch.  Future releases of the HiTechnic EV3 blocks will include Comparison modes so that the 
sensors can be read directly from Wait, Loop and Switch blocks.

Installation:
1) Extract the .ev3b files from the downloaded zip file.  
2) From the LEGO EV3 software, select Block Import from the Tools menu.
3) From Block Import, Browse to the folder where the HiTechnic .ev3b files are located.
4) Select the desired .ev3b file.
5) Click Import.
Repeat steps 4 and 5 for all other blocks you want to import.
6) Restart the EV3 software.

ZIP file also includes sample EV3 programs that display the sensor value.  All three programs assume the
sensor used is on sensor port 1.

Note that currently only the EV3 is supported.  The blocks will not work with the NXT.


