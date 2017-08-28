# UDP-socket-connection

Steps to compile and  execute:

	UDP module:
	-To compile server module:
		-gcc rcmdd.c -o rcmdd
	The executable is stored in rcmdd
	-To compile client module:
		-gcc rcmd.c -o rcmd
	The executable is stored in rcmd

	-To execute server module:
		-./rcmdd <portnumber> 
	-To execute client module:
		-./rcmd <serverIP> <serverport> <executing_count> <time_delay> <command>
    
