#Code for Phytec Board
Source Code for reading the sensors 
##compile instructions
In line 51 the user can decide if the sensor node is outside of thewindow or inside.
To compile the source code you must export the path to gcc-arm in the folder.
For example:

export PATH=$PATH:/home/riot-os-devel/Schreibtisch/Develop/gcc-arm-none-eabi-4_9-2015q2/bin

After this step you must compile the source code with the following command:
BOARD=pba-d-01-kw2x make
To flash the code to the phytec board you must write:
BOARD=pba-d-01-kw2x make flash
If you want to read the output on the serial communication type:
BOARD=pba-d-01-kw2x make term
