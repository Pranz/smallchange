Build instructions
----------------
 - The dependencies:
 	- boost (C++ libraries)
 	- db48 (Berkeley DB 4.8)
 	- qt4 (Open Source QT 4.8.4, includes qmake)
 	- openssl (ssl dev libraries)
 	- miniupnpc (UPNP dev libraries)
	
 - Linux
 	
	- Install the dependencies:
 		- `apt-get install build-essential libboost-dev libminiupnpc-dev libdb++-dev libssl-dev libdb++-dev qt-sdk`

	- Create the make file:

		 - Inside toakrona-master, type:

			 - `qmake "USE_UPNP=-"`

		Expected output:
		- Project MESSAGE: Building without UPNP support Removed plural forms as the target language has less forms. 	If this sounds wrong, possibly the target language is not set or recognized.
 
	- Compile:

		- `make`

 - Mac OS X
 
	- Install MacPorts
	
	- Install the dependencies
		- ` %sudo port install boost db48 qt4-mac openssl miniupnpc`
		
	- Create the make file:
		- inside toakrona-master input:
			- `qmake "USE_UPNP=-" toakrona-qt.pro`
		
		 - Expected output:
			- Project MESSAGE: Building without UPNP support Removed plural forms as the target language has less forms. If this sounds wrong, possibly the target language is not set or recognized.
 
	- Compile:
		 - `make -f Makefile`


