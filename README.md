drone_simulator
===============
* Requires FFMpeg with libx264 enabled
* Tested on Arch Linux and Mac OS X

Installation:
-------------
* Clone the repository
* Run make in the root of the repository

Usage:
------
* Run setup_test_forwarding.sh in the root directory on the computer running the client software (changing 127.0.0.1 to the IP address of the computer running the server software).
* cd to the bin/ directory and run ./server.a with any combination of the following options:

		-h		Print this help text.
		-v		Get video stream from v-rep (requires v-rep to be running).
		-w <filename>	Get video stream from camera specified by filename. If no filename specified, defaults to
				/dev/video0 (does not work on OS X).
		-c {vrep|print}	Use the specified method to deal with control commands. print will print out command parameters
				while vrep will send control commands to v-rep to be processed by the simulation.
		-n {vrep}	Use the specified source of navigation data. Right now, only v-rep is supported.
		
* To use with v-rep, you must ensure that v-rep is running on the same computer as the server software (it can work on a different computer if you change the IP address in the source code. However, it does not work reliably). Then, open the dronescene.ttt file from the vrep_drone_scripts folder and run the simulation.

Adding modules:
---------------
The system is created to be easily extendable. In order to handle a specific situation differently (e.g. a different video source or different code for handling control signals), all that is needed is the addition of an init function which needs to be called in main() when command line arguments are being parsed and the relevant handler function, to which a pointer is added to the data_options struct in your init function.
