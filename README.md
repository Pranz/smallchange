Toakrona (TOA)
================================

http://toakrona.flygande-toalett.tk

Copyright (c) 2009-2013 Bitcoin Developers

Copyright (c) 2011-2013 Litecoin Developers

Copyright (c) 2014-2016 Toakrona Swedes

What is Toakrona?
----------------

It is a crypto currency intended to be used in #toa@flygande-toalett.tk

Toakrona is a direct clone of Litecoin with some small changes.
 - scrypt as a proof-of-work algorithm like Litecoin
 - 2 minute block targets
 - 13.37 million total coins
 - 50 coins per block

Build instructions
----------------
 - Install the following dependencies:

 	- boost (C++ libraries)
 	- db48 (Berkeley DB 4.8)
 	- qt4 (Open Source QT 4.8.4, includes qmake)
 	- openssl (ssl dev libraries)
 	- miniupnpc (UPNP dev libraries)

 - Create the make file:

	Inside toakrona-master, type:

	`qmake "USE_UPNP=-"`

	Expected output:
 	- Project MESSAGE: Building without UPNP support Removed plural forms as the target language has less forms. 	If this sounds wrong, possibly the target language is not set or recognized.
 
 - Compile:

	`make`



License
-------

Toakrona is released under the terms of the MIT license. See `COPYING` for more
information or see http://opensource.org/licenses/MIT.
