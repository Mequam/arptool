# arptool
A tool for creating custom arp requests using the linux packet socket type

arptool is dependent upon a c++ parsing library called cparse, make sure that it is installed
on your system before you try compiling!

to compile arptool simply type "make build" in the same directory as the source files
and the program should compile for you

arptool help page coming soon! For now heres a list of flags that you can feed the program
to affect the arp request

-ip: set the ip that the arp request querys

-Sip: set the source ip address in the arp request

-Sm: set the source mac address in the arp request

-r: toggle responce mode, causes the program to wait for a responce from the arp ping

-v: toggle verbose mode, causes more verbose output from the program
