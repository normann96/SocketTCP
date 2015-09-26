#include <conio.h>
#include <stdio.h>
#include <iostream>
// #include <WinSock2.h>
// #include <ws2tcpip.h>	

// #pragma comment(lib, "wsock32.lib")
// #pragma comment(lib, "ws2_32.lib")


void SocketGetHostName()
{
	char chInfo[64];

	if (gethostname(chInfo,sizeof(chInfo)))
	{
		printf("Not local host\n");
		exit(1);
	}
	else
	{	
		printf(chInfo);
		printf(" GetHostName.\n");
	}
}
