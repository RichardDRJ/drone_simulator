drone_simulator
===============
Dependencies:
-------------
* FFMpeg with libx264 enabled
* Tested on Arch Linux and Mac OS X

Installation:
-------------
* Clone the repository
* Run make in the root of the repository

Usage:
------
* cd to the bin/ directory and run ./server.a with any combination of the following options:

		-h				Print this help text.
		-v				Get video stream from v-rep (requires v-rep to be running).
		-w <filename>	Get video stream from camera specified by filename. If no filename specified, defaults to
						/dev/video0 (does not work on OS X).
		-c {vrep|print}	Use the specified method to deal with control commands. print will print out command parameters
						while vrep will send control commands to v-rep to be processed by the simulation.
