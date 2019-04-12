char hex2char(unsigned char c)
{
	switch(c)
	{
		case '\x00':
			return '0';
		case '\x01':
			return '1';
		case '\x02':
			return '2';
		case '\x03':
			return '3';
		case '\x04':
			return '4';
		case '\x05':
			return '5';
		case '\x06':
			return '6';
		case '\x07':
			return '7';
		case '\x08':
			return '8';
		case '\x09':
			return '9';
		case '\x0A':
			return 'A';
		case '\x0B':
			return 'B';
		case '\x0C':
			return 'C';
		case '\x0D':
			return 'D';
		case '\x0E':
			return 'E';
		case '\x0F':
			return 'F';
		default:
			printf("[*] returning the default case\n");
			return '0';
	}
}
void getmac(unsigned char* src, char* dest)
{
	for (int i = 0; i < 6;i++)
	{
		dest[i*3]=hex2char((src[i]-src[i]%0x10)/0x10);
		dest[i*3+1]=hex2char(src[i]%0x10);	
		if (i < 5) {dest[i*3+2]=':';}
	}
}
