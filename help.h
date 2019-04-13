/*
 this file contains the help message that the program prints out to the screen when the user gives them a -h flag
 issolated off by itself for cleanlyness in case the help message gets too long
 in the future perhaps we should add in an option to generate a help message from the parsing library?
*/
void printHelp()
{
	printf("usage: arptool [flag1 [flag2 [flag3...]]]\n");
	printf("FLAGS:\n");	
	printf("---------------------------------------\n");
	printf("-h.....................print this message and exit\n");	
	printf("-ip.................the target ip address to query\n");
	printf("-Sip..................the source ip address to use\n");
	printf("-Sm....the source macc address to use in the query\n");
	printf("-v...........................toggle verbose output\n");
	printf("-q.............toggle quiet output, overides above\n");


}
