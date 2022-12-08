Project: IrChat
Version: 1.0
Programmer: Supareak V. (ng@jimmy.com)
Copyright (c): Art & Technology Co.,Ltd.
Last update: Dec 13, 1998

----------------------------------------------------------------------
IrChat 1.0 is a small utility program that allow you to exchange typing text between 2 H/PCs by using the build-in infared device port. Basiclly, the infared port is came with all H/PCs. The first version still have some bugs.

After you start the program, the screen will show a black horizentol line locate at middle of the screen. The upper-half is the area for recieved text and other is for your typed text. Choose "Connect" command from the "Connect" menu to start the communication. A dialogbox will appers to prompt the next operation. Position two H/PCs in range and click OK. If the program run with out any problem, two H/PCs are able to chat. If not, the program will be stop and you need to reset the machine.

The source code has compiled with Microsoft VC/C++ SDK 5.0 for Windows CE platform. The files in the project are:

	comm.cpp - infared port device driver routines
	termimal.cpp - a simple terminal screen emulator
	IrChat.cpp - main Windows routines and program sktelation
	IrChat.rc - resource file
	IrChat.ico - icon file
	IrChat.h - C/C++ header file
	IrChat.dsw - developer studio workspace file
	IrChat.* - developer studio project files
	Readme.txt - detail about the project
	WMIPSRel\IrChat.exe - executabe file for MIPS machines
	WCESH3Rel\IrChat.exe - executable file for SH3 machines

The source code are not look hard to understand. Don't ask me for any support or service.

--------------------
Supareak V.
ng@jimmy.com
