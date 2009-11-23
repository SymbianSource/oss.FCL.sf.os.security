Two version of openssl libraries :

1)	Windows(VC++)

	Linking		:	Statically linked library
	Extension	:	.lib

2)  Windows(MinGW)

	Linking		:   Statically linked library
	Extension	:	.a
				
How to build openssl library for MinGW ?
				
1)Download the source code for openssl 9.7b version from http://www.openssl.org/ openssl-0.9.7b.tar.gz (currently we support 9.7b)

2) 	untar it using  tar -zxvf openssl-0.9.7b.tar.gz

3)	release version :
	run ms/mingw32.bat in cygwin : It will generate  libcrypto.a in /out directory, which is a static version (Release) of openssl.

4)	debug version :

	a)	replace  line  " perl util/mk1mf.pl gaswin  Mingw32 >ms/mingw32a.mak "  with "perl util/mk1mf.pl gaswin debug Mingw32 >ms/mingw32a.mak"
	b)	run ms/mingw32.bat in cygwin : it will generate libcrypto.a in /out.dbg directory which is a static version (Debug) of openssl.
