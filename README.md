MemoryManagement
================

Implementation a Simple Virtual Memory Manager Simulator

Compiling with makefile.
	- Move to the folder of the server in terminal.
	- Type "make" and press return.
	- Type ./virtualmem <options> and press return.
	- To see options type ./virtualmem -h and press return.

Compiling without makefile.
	- Type gcc virtualmem.c -o virtualmem -lm
	- Type ./virtualmem <options>

Input sequence should be of the following form.
2 3 4 1 2 7 8 4 or 23412784
- Each number can be separated by a space or not.
- Page number should be of single digit.
