Here you can download a proxy software which allows the Siemens D500 x-ray diffractometer to be upgraded with add-ons that are not officially available for it. In its current state the software is used to operate a D500 with an automatic attenuator.


For more information, including an example of successful operation, please refer to our publication:

"Automatic attenuator upgrade for a Siemens D500 diffractometer via a generic software library to overcome hardware limitations", Nuclear Instruments and Methods in Physics Research A 855 (2017) 61–64, 
http://dx.doi.org/10.1016/j.nima.2017.02.088

If you use our software for obtaining scientific results, we kindly ask you to quote the publication.

-----------------------------------------------------------------------

Copyright (C) 2016  Christian Randau, Sina Mayr, Wolfgang Kreuzpaintner

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. 
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. 
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.


-----------------------------------------------------------------------
Dependencies:
-----------------------------------------------------------------------

The "Diffractometer-Proxy" is a C++ application based on Qt4 and QExtSerialPort. The used IDE was VS2010 and the solution and project files are included. 
For the compilation Qt4 must be installed and the environment variable "QTDIR" must be set to the installation folder of Qt4. 
Furthermore the serial port extention QExtSerialPort for Qt4 is used and must be accessable. It can be downloaded from https://github.com/qextserialport/qextserialport. Please update the additional directory of the VS projects to link to your QExtSerialPort directory.

-----------------------------------------------------------------------
Important settings:
-----------------------------------------------------------------------

1) The Baud rates and COM ports in the files parameterClientD500.ini and parameterClientD5000.ini have to be set accordingly to the settings of the diffractometer and the XRDCommander

2) The offset values for each drive (set when a ZI command is sent to the diffractometer) in the file offsetParameter.txt have to be the same as the reference positions in the config file (i.e. DIFF_IN1.CNF, found in the XRDCommander directory)

In DIFF_IN1.CNF the relevant lines would for example look similar to the following:
[Drive 1]
fReferencePosition=-2.00000
...

(The value -2.00000 is exemplary and must be adjusted to your individual value. The positions have to be set for all available drives of your D500 with their individual values each).

3) If a stop command (HT) is sent to the D500 diffractometer, any motion stops. If the speed of the motion is greater than the speed set via the period slow command (PS) the initialization is lost. Since HT commands are needed to operate the attenuator, it is advisable to set the fast speed of the diffractometer to the same as the slow speed. Then the initilization will not be lost. The speed can be set in set-up mode via PF and PS commands.

For example, this is achieved by sending the following commands in the "direct mode" to the D500:

SU
PF2600,2600,2500,2500
PS2600,2600,2500,2500
QU

(SU enters the set-up mode, QU ends it. The PF and PS values shown are the highest possible values found working)


-----------------------------------------------------------------------
- Some things have to be changed manually in the config file (i.e. DIFF_IN1.CNF, found in the XRDCommander directory) for operating a D500 as a D5000:
-----------------------------------------------------------------------

1) The type of the diffractometer has to be changed:

[Basic Diffractometer Settings]
sDiffractometerType=D500 	--> change to --> 	sDiffractometerType=D5000

	
2) If you have a software operated x-ray shutter, [Tube Window 1] has to be changed:

[Tube Window 1]
sWindowType=D500 Shutter	--> change to -->	sWindowType=Low Speed Shutter


3) [Absorber 1] has to be activated: set the absorber factor and (de)activation limits in accordance with your set-up. 

For example if the attenuation factor is 89.93 (fAbsorberFactor2 and fAbsorberFactor3 can be ignored as they are only needed for 4-step attenuators. We have a 2-step attenuator, here.):

[Absorber 1]
sStatus=used
sType=Slit Changer
fActivationLimit=100000
fDeactivationLimit=90000
iNumberOfPositions=2
fAbsorberFactor0=1
fAbsorberFactor1=89.93
fAbsorberFactor2=100
fAbsorberFactor3=10000
iDriveNumber=20
