#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <conio.h>

#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "ws2_32.lib")

#define Con_color(a) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), a)

using namespace std;

SOCKET Client = {0};
char buff[1024] = "\0";
int nData = 0;
#define  PORT 8090

DWORD WINAPI ThreadSendToServer(void *pV);

HANDLE hThreadShow = {0};
DWORD dwThr = 0;

int main()
{
//	setlocale(0, "");

	WSADATA wsd;
	WSAStartup(MAKEWORD(2,2), &wsd);

	//  	char **pptr;
	//  	struct hostent *hptr;
	//  	struct in_addr sin_addr;
	//  
	//  	hptr = gethostbyname("www.alexandro.com.ua");
	//  	pptr = hptr->h_addr_list; // список ip адресов
	//  
	//  	for (; *pptr != NULL; pptr++)
	//  	{
	//  		CopyMemory(&sin_addr, *pptr, hptr->h_length); //  опируем IP адрес в структуру sin_addr
	//  		cout << inet_ntoa(sin_addr) << endl;
	//  	}

	Client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in AddresClient;
	AddresClient.sin_family = AF_INET;
//	AddresClient.sin_addr.s_addr = inet_addr("10.1.129.15");
	hostent *hptrAdrr = gethostbyname("www.alexandro.com.ua");
	memcpy(&AddresClient.sin_addr, hptrAdrr->h_addr_list[0], hptrAdrr->h_length);
	Con_color(10);
	cout << "Trying to connect a: " << inet_ntoa(AddresClient.sin_addr) << " server"<< endl;
	Con_color(15);
	AddresClient.sin_port = htons(PORT);


	int res = connect(Client, (sockaddr*)&AddresClient, sizeof(AddresClient));
	if (res == SOCKET_ERROR)
	{
		Con_color(12);
		cout << "Error connection\n";
		closesocket(Client);
		Sleep(3000);
		return -1;
	}

	while(true)
	{

		hThreadShow = CreateThread(0, 0, ThreadSendToServer, 0, 0, &dwThr);
		if (hThreadShow == NULL)
		{
			Con_color(12);
			cout << "Error ThreadFromServer\n";
			return -1;
		}

		nData = recv(Client, buff, sizeof(buff), 0);
		if (nData == SOCKET_ERROR)
		{
			Con_color(12);
			cout << "Error. Server offline\n";
			shutdown(Client, 2);
			//		TerminateThread(hThreadShow, 0);

			ExitThread(dwThr);
			Sleep(1000);
			return 0;
			exit(1);
		}
		else
		{
			buff[nData] = '\0';
			Con_color(8);
			cout << "From server: ";
			Con_color(15);
			cout << buff << endl;
			Con_color(15);
		}
	}

	//----------------------------------------------------------------------

	//----------------------------------------------------------------------

	closesocket(Client);
	WSACleanup();

	system("pause");
	return 0;
}

DWORD WINAPI ThreadSendToServer( void *pV )
{
	cin.getline(buff, 1024);
	int tmp = send(Client, buff, sizeof(buff), 0);
	if (tmp == 0)
	{
		cout << "Server offline\n";
		Con_color(15);
		return -1;
	}

	return 0;
}
