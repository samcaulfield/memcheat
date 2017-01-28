memcheat

A tool for messing with the memory of other processes.

This tool currently only works on new-ish versions of Linux or other operating
systems that happen to have both the exact same APIs and procfs.

Usage:
	`search <value>` - Searches the process's stack, heap and writable data
	section for the value. All addresses containing the value are printed.
	The value is assumed to be an unsigned 32-bit integer.

	`write <value> <address>` - writes the value to the address in the
	process's virtual address space. memcheat will probably need to be run
	with root privileges for this to work.

Using these two commands, it's sometimes possible to figure out the location of
a variable in the process's virtual address space. You can change the variable
in the process itself (e.g. by performing some user interface action in the
program) and then re-scan the process for the value. Once only one memory
address is being printed for the value consistently you are quite likely to have
found the location, and then the fun begins. If the program uses some kind of
clever obfuscation of variables then you're probably out of luck.

I'll probably add the ability to search for different kinds of data types, e.g.
ASCII-encoded strings and IEEE-754 floating-point numbers.

Sometimes it's possible to find the memory address of a variable without
knowing its exact position at any time. This can be done by observing changes
in values over time. To give a simple example, in the game Pong, one could
potentially find the location of the variable containing the Y position of the
ball by looking for memory locations that only contain increasing values over
some manually-defined time interval. I'll probably add some kind of mode for
doing this.

