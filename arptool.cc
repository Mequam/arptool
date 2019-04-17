#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

//this is a custom library file that parses out user data
//to use it the program needs to be compiled with the -lparse flag
#include <parse.h>
#include "strmac.h"
#include "help.h"
#include "version.h"

int makeIp(void* src,void* dest)
{
	struct in_addr ip_addr;
	if (inet_aton((char*)src,(in_addr*)&ip_addr) != 1)
	{ 
	  printf("\033[1;31m[ERROR]\033[0;31m Invaid ip address!\n");
	  exit(EXIT_FAILURE);
	}
	//store the parsed out ip address in our given destination
	*(uint32_t *)dest=ip_addr.s_addr;
	return 1;
}
unsigned char hex2char(char c) 
{
	if ( 0x40 < c && c < 0x47)
	{
		//we have a valid capitol asci hex letter	
		return c-55;

		
	}
	else if (0x60 < c && c < 0x67) 
	{
		//we have a valid lowercase ascii hex letter
		return c-87;
	}
	else if (0x2F < c && c < 0x3A)
	{
		//we have a valid ascii number representation letter	
		return c-48;


	}
	//we have nothing valid left, return -1
	return -1;
}
int setSourceMac(void* src,void* dest)
{
	char out[2] = {'a','\x00'};
	//set the macc address when given a valid macc
	//XX:XX:XX:XX:XX:XX
	//multiply i by 3 becuse we want i to allways land on the start of a valid ascii hex representation
	for (int i = 0;i<6;i++)
	{
		//if we find the end of the string before parsing all the way through we found an invalid macc
		if (((char*)src)[i*3] == '\x00' || ((char*)src)[(i*3)+1] == '\x00')
		{ 
			//we hit the end of the string before we parsed out all of the values
			printf("\033[1;31m[ERROR]\033[0;31m reached the end of macc address without parsing out the macc, exinting!\n");
			exit(EXIT_FAILURE);
		}
		//parse out the hex bytes from the ascii representation, will return -1 on failure
		char a = hex2char(((char*)src)[i*3]);
		char b = hex2char(((char*)src)[(i*3)+1]);
		if (a == -1 || b == -1)
		{
			//they gave us an invalid hex charicter, die
			printf("\033[1;31m[ERROR]\033[0;31m Invalid hex byte detected, exiting!\n");
			exit(EXIT_FAILURE);
		}
		//set the destination index to our desired hex value using some simple hex math
		//we dont multiply this times three sense we the two hex values from the ascii convert to one hex value
		//in our destination (that and we ignore the : in the actual representation)
		out[0] = a*16+b;
		((char*)dest)[i] = a*16+b;
		
	}
	return 1;
}

int main(int argc, char ** argv) {



//parsing code
	//ip address that we query
	uint32_t ip;
	struct arg ip_arg;
	ip_arg.flag="-ip";
	ip_arg.type=CUSTOM;
	ip_arg.store=&ip;
	ip_arg.parser=&makeIp;
	
	//source ip address that we use for ours protocol addr
	uint32_t sip;
	struct arg sip_arg;
	sip_arg.flag="-Sip";
	sip_arg.type=CUSTOM;
	sip_arg.store=&sip;
	sip_arg.parser=&makeIp;

	//the net device that we use in our arp query
	char netdev[11];
	memcpy(netdev,"wlo1",5);
	struct arg netdev_arg;
	netdev_arg.flag="-d";
	netdev_arg.type=STRING;
	netdev_arg.store=netdev;
	netdev_arg.size=11;
	
	//the source mac address that we use in the query
	unsigned char source_eth_addr[6]={0x00,0x00,0x00,0x00,0x00,0x00};
	struct arg seadd_arg;
	seadd_arg.flag="-Sm";
	seadd_arg.type=CUSTOM;
	seadd_arg.store = (void*)source_eth_addr;
	seadd_arg.parser=&setSourceMac;

	unsigned char target_eth_addr[6]={0x00,0x00,0x00,0x00,0x00,0x00};
	struct arg teadd_arg;
	teadd_arg.flag="-m";
	teadd_arg.type=CUSTOM;
	teadd_arg.store = (void*)target_eth_addr;
	teadd_arg.parser=&setSourceMac;

	//do we want to output when the program changes defaults
	bool verbose;
	struct arg verb_arg;
	verb_arg.store=&verbose;
	verb_arg.flag="-v";
	verb_arg.type=BOOL;
	
	//do we want to output EVERYTHING
	bool superVerbose;
	struct arg superVerbose_arg;
	superVerbose_arg.flag="-vv";
	superVerbose_arg.type=BOOL;
	superVerbose_arg.store=&superVerbose;

	//do we want to recive a responce?
	bool rec;
	struct arg recv_arg;
	recv_arg.type=BOOL;
	recv_arg.flag="-r";
	recv_arg.store=&rec;

	//does the user want to turn output off?
	bool quiet=false;
	struct arg quiet_arg;
	quiet_arg.type=BOOL;
	quiet_arg.flag="-q";
	quiet_arg.store=&quiet;

	bool version=false;
	struct arg vs_arg;
	vs_arg.type=BOOL;
	vs_arg.flag="-V";
	vs_arg.store=&version;
	struct arg v_arg;
	v_arg.type=BOOL;
	v_arg.flag="--version";
	v_arg.store=&version;


	//make lots of flags that point to the help argument so that way if a new user is floundering around
	//they have a better chance of guessing the right help flag
	bool help=false;
	struct arg help1_arg;
	help1_arg.type=BOOL;
	help1_arg.flag="-h";
	help1_arg.store=&help;
	struct arg help2_arg;
	help2_arg.type=BOOL;
	help2_arg.flag="--help";
	help2_arg.store=&help;
	struct arg help3_arg;
	help3_arg.type=BOOL;
	help3_arg.flag="-?";
	help3_arg.store=&help;
	
	bool at=false;
	struct arg at_arg;
	at_arg.flag="-a";
	at_arg.type=BOOL;
	at_arg.store=&at;

	struct arg* destv[15] = {&teadd_arg,&superVerbose_arg,&at_arg,&help1_arg,&help2_arg,&help3_arg,&vs_arg,&v_arg,&quiet_arg,
		&ip_arg,&sip_arg,&netdev_arg,&seadd_arg,&verb_arg,&recv_arg};
	parse(argc,argv,15,destv);

//end of parsing code
	//user wants to print out a help message
	if (help) 
	{	printHelp();
		exit(EXIT_SUCCESS);
	}
	//the user wants to print out the version of the applicantion, print the version and exit
	if (version)
	{
		printVersion();
		exit(EXIT_SUCCESS);
	}
	
	
	//make the packet socket and die if the user does not have the proper privlages
	//no need to run the rest of the program if we dont have privlages
	int fd = socket(AF_PACKET,SOCK_DGRAM,htons(ETH_P_ALL));
 	if (fd == -1)
	{
		printf("\033[1;31m[ERROR]\033[0;31m root privliges required!\n");
		exit(EXIT_FAILURE);
	}

	//the quiet flag overides the verbose flag to turn it off
	if (quiet) {verbose=false;superVerbose=false;}

	if (verbose || superVerbose)
	{
		//output changes to the netdevice
		if (netdev_arg.set)
		{
			printf("\033[1;32m[*]\033[0;00m set netdevice to %s\n",netdev);
		}
		else if (superVerbose)
		{
			printf("\033[1;32m[*]\033[0;00m using default netdevice of wlo1\n");
		}
		//output the changes to the query address based on verbose flags
		if (ip_arg.set)
		{
			//TODO:make it so that we can output an actual ip address that we are seting
			printf("\033[1;32m[*]\033[0;00m setting target protocol address to 0x%x\n",ntohl(ip));
		}
		else if (superVerbose)
		{
			//TODO:it would be really cool to make this default to the default gateway
			//but there would be much learning before that point
			printf("\033[1;33m[*]\033[0;33m query address is not set, using 0\n");
		}
		if (teadd_arg.set)
		{
			char strmac[18];
			strmac[17] = '\x00';
			getmac(target_eth_addr,strmac);	
			printf("\033[1;32m[*]\033[0;00m set the target hardware address to [%s]\n",strmac);
		}
		else if (superVerbose)
		{
			printf("\033[1;32m[*]\033[0;00m defaulting target hardware address to blank [00:00:00:00:00:00]\n");

		}
	}

	
	
	//initilise our ifreq interface for use with the linux ioctl commands
	struct ifreq ifr;
	memcpy(ifr.ifr_name,netdev,strlen(netdev)+1);
	if (ioctl(fd,SIOCGIFINDEX,&ifr) == -1)
	{
	 printf("[ERROR] unable to retrieve interface index, godspeed user\n");
	 exit(EXIT_FAILURE);
	}

	//create an address that we can use with the packet interface to send low level data
	//using the device number we just got
	struct sockaddr_ll addr = {0};
	addr.sll_family = AF_PACKET;
	addr.sll_ifindex = ifr.ifr_ifindex;
	addr.sll_protocol = htons(ETH_P_ARP);	
	addr.sll_halen = ETHER_ADDR_LEN;
	
	memcpy(addr.sll_addr,target_eth_addr,sizeof(target_eth_addr));

	
	
	
/*construct our ethernet request*/	
	struct ether_arp req;
	memset(&req,0x00,sizeof(req));
	req.arp_hrd = htons(ARPHRD_ETHER);
	req.arp_pro = htons(ETH_P_IP);
	req.arp_hln = ETHER_ADDR_LEN;
	req.arp_pln = sizeof(in_addr_t);
	//what mode do we want to be in?
	if (at)
	{

		req.arp_op=htons(2);
		if (verbose || superVerbose) 
		{
			printf("\033[1;32m[*]\033[0;00m set operation to send and listen\n");
		}

	}
	else
	{
		req.arp_op=htons(1);
		if (superVerbose) 
		{
			printf("\033[1;32m[*]\033[0;00m set operation to default, send\n");
		}

	}

	//this sets the target hardware address to 0, as thats what we are going to end up asking for
	memset(req.arp_tha,0x00,sizeof(req.arp_tha));
	//set the target ip address to the given ip
	memcpy(req.arp_tpa,&ip,sizeof(ip));
	//set the source hardware address
	if (!seadd_arg.set) 
	{
		//default to using the given network cards address
		ioctl(fd,SIOCGIFHWADDR,&ifr);
		memcpy(req.arp_sha,(unsigned char*) &ifr.ifr_hwaddr.sa_data,sizeof(req.arp_sha));
		if (superVerbose) 
		{
			//this code is for formating the mac address to show the user
			char strmac[18];
			strmac[17] = '\x00';
			getmac(req.arp_sha,strmac);
			printf("\033[1;32m[*]\033[0;00m set source hardware address to default, interface address [%s]\n",strmac);
		}

	}
	else
	{
		memcpy(req.arp_sha,source_eth_addr,sizeof(req.arp_sha));
		if (verbose || superVerbose) 
		{
			//this code is for formating the mac address to show the user
			char strmac[18];
			strmac[17] = '\x00';
			getmac(source_eth_addr,strmac);
			printf("\033[1;32m[*]\033[0;00m set source hardware address to [%s]\n",strmac);
		}

	}
	//set the source protocol address
	if (!sip_arg.set)
	{	//default to using the given network cards ip address
		ioctl(fd,SIOCGIFADDR,&ifr);
		memcpy(req.arp_spa,ifr.ifr_addr.sa_data+2,sizeof(req.arp_spa));
		if (superVerbose) {printf("\033[32;1m[*]\033[00;0m set source protocol address to %s's address\n",netdev);}
	}
	else
	{       //they supplied us with a source address, store it into the request
		memcpy(req.arp_spa,&sip,sizeof(req.arp_spa));
		if (verbose || superVerbose) {printf("\033[33;1m[*]\033[33;0m set source protocol address to user defined value\n");}
	}

	//send the arp request
	if (verbose || superVerbose)
	{
		printf("\033[33;1m[*]\033[0;33m sending arp request...\n");
	}
	if (sendto(fd,&req,sizeof(struct ether_arp),0,(struct sockaddr* )&addr,sizeof(struct sockaddr_ll)) == -1 )
	{
		printf("\033[1;31m[ERROR]\033[0;31m could not send message!\n");
		exit(EXIT_FAILURE);
	}

	//listen for a responce if they set the rec flag
	if (rec)
	{
		struct ether_arp resp;
		if (verbose || superVerbose)
		{
			printf("\033[1;33m[*]\033[0;33m listinging for responce...\n");
		}
		recv(fd,&resp,sizeof(struct ether_arp),0);
		
		//format and output the recived ip address to the screen
		char macstr[18];
		macstr[17] = '\x00';	
		getmac(resp.arp_sha,macstr);
		printf("\033[1;32m[*]\033[0;32m recived reply from [%s]\n",macstr);
	}
	if (!quiet)
	{
		//we are not quiet the user wants to output inforamation
		printf("\033[1;32m[*]\033[0;32m completed successfully! :D\n\033[0;00m");
	}
	return 1;

}
