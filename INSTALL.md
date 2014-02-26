Build instructions
----------------
- The dependencies:
	- boost (C++ libraries)
	- db48 (Berkeley DB 4.8.30)
	- qt4 (Open Source QT 4.8.4, includes qmake)
	- openssl (ssl dev libraries)
	- miniupnpc (UPNP dev libraries)
	
- GNU/Linux: Debian derived
	
	- Install the dependencies:
		- `apt-get install build-essential libboost-dev libminiupnpc-dev libdb++-dev libssl-dev libdb++-dev qt-sdk libboost-all-dev`

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


- Windows (Cross compiling from Linux)

	- Using mxe (http://mxe.cc/)
		Download mxe and build it:

			git clone https://github.com/mxe/mxe.git mxe
			cd mxe
			make
			make qttools

		Download db 4.8.30.NC and build it for mxe:

			cd <insert path to db-4.8.30.NC>
			mkdir build_mxe
			cd build_mxe
			export PATH=<insert path to mxe directory>/usr/bin/:${PATH};CC=i686-pc-mingw32.static-gcc CXX=i686-pc-mingw32.static-g++ ../dist/configure --disable-replication --enable-mingw --enable-cxx --host x86_64 --prefix=<insert path to mxe directory>/usr/i686-pc-mingw32.static
			make
			make install

		Download toakrona:

			git clone https://github.com/Pranz/toakrona.git toakrona
			cd toakrona

		toakrona-qt building (Assuming we don't want upnp support) (It will be built to the "release" directory):

			<insert path to mxe directory>/usr/i686-pc-mingw32.static/qt5/bin/qmake "USE_UPNP=-"
			make
