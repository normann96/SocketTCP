#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <conio.h>
#include <ws2tcpip.h>	

#include "Function.h"

#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define Con_color(a) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), a)

SOCKET ServerSocket = {0};
SOCKET NewClientSocket = {0};
SOCKET * pNewConnectSocket = nullptr;

HANDLE hThread = {0};
HANDLE hThreadSock = {0};

sockaddr_in ServAddress;
sockaddr_in ClientAddress;

int saSize = sizeof(ServAddress);
int caSize = sizeof(ClientAddress);

sockaddr_in TestAddres;
int caTestSize = sizeof(TestAddres);

short int PORT = 8090;
int nData = 0;
int Ret = 0;
char buff[1024];
int ClientCount = 0;

char messegeClient[] = "You successfull connected on server!";

DWORD WINAPI ThreadSendReceiveFunc(void * pV);
DWORD WINAPI ThreadFuncAcceptConnection(void *pV);
DWORD WINAPI ThreadMessAll(void *pV);

void ShowInfoClient();

int main()
{
	setlocale(0, "");

	SetConsoleCP(1251);				// установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251);		// установка кодовой страницы win-cp 1251 в поток вывода

// Инициализация Winsock версии 2.2

	WSADATA wsd;
	if(Ret = WSAStartup(MAKEWORD(2,2), &wsd) != 0)
	{
		Con_color(12);
		cout << "Error WSAStartup: " << WSAGetLastError() << endl;
		Sleep(3000);
		return -1;
	}
	else
	{
		Con_color(14);
		cout << "Initial Startup!\n";
		Con_color(15);
	}
	
	pNewConnectSocket = new SOCKET[50];

// Создаём новый TCP сокет для приёма запросов на соединение от клиентов.

	if((ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		Con_color(12);
		cout << "Error Server socket: " << WSAGetLastError() << endl;
		WSACleanup();
		Sleep(3000);
		return -1;
	}

// Заполняем   struct sockaddr_in ServerAddr, которая скажет функции bind, что мы хотим
// слушать соединения на всех интерфейсах (INADDR_ANY), используя порт 8090. 
// Мы преобразуем порядок байт  из системного в сетевой (htons и htonl)

	ZeroMemory(&ServAddress, sizeof(ServAddress));
	ServAddress.sin_family = AF_INET;
	ServAddress.sin_port = ntohs(PORT);
	ServAddress.sin_addr.s_addr =  INADDR_ANY;

// Функция  bind привязывает адресную информацию, определённую  в ServAddress к сокету  ServerSocket

	if(bind(ServerSocket, (sockaddr*)&ServAddress, sizeof(ServAddress)) == SOCKET_ERROR)
	{
		Con_color(12);
		cout << "Error bind: " << WSAGetLastError() << endl;
		closesocket(ServerSocket);
		WSACleanup();
		Sleep(3000);
		return -1;
	}

// Делаем сокет пасивным для прослушивания (приёма) запросов на TCP-соединение от клиентов.
// Длина очереди запросов  на соединение равна maximum

	if(listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		Con_color(12);
		cout << "Error listen: " << WSAGetLastError() << endl;
		closesocket(ServerSocket);
		WSACleanup();
		Sleep(3000);
		return -1;
	}
	else
	{
		Con_color(14);
		cout << "Server start. Waiting connection...\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	}

// Создаем поток для приема новоего соединения, когда оно возникнет

	hThread = CreateThread(0, 0, ThreadFuncAcceptConnection, &ClientAddress, 0, 0);
	if (hThread == NULL)
	{
		cout << "Error thread " << GetLastError() << endl;
	}
	WaitForSingleObject(hThread, INFINITE);

	closesocket(ServerSocket);
	closesocket(NewClientSocket);
//-------------------------------------------------------------------------
	WSACleanup();
	delete [] pNewConnectSocket;

	system("pause");
	return 0;
}

DWORD WINAPI ThreadSendReceiveFunc(void * pV)
{
	int ID = (int)pV;

	while(true)
	{
		if((nData = recv(pNewConnectSocket[ID], buff, sizeof(buff), 0)) == SOCKET_ERROR)
		{
			cout << "User Num." << ID+1 << "  offline\n";
			memset(&buff, 0, sizeof(buff));
			closesocket(pNewConnectSocket[ID]);
			NewClientSocket = 0;
			Sleep(3000);
			return -1;
		}
		if (nData != 0)
		{
			CreateThread(0, 0, ThreadMessAll, 0, 0, 0);
			Con_color(8);
 			cout << "Server received " << "from user Num." << ID+1 << ": ";
			Con_color(15);
			cout << buff << endl;
			Con_color(15);
		}
	}
	return 0;
}

DWORD WINAPI ThreadFuncAcceptConnection( void *pV )
{
	// Server accept

	while( true /*(NewClientSocket = accept(ServerSocket, (sockaddr*)&ServAddress, &saSize)) > 0*/ ) 
	{
// Принимаеи новое соединение, когда оно возникнет

		if ((NewClientSocket = accept(ServerSocket, (sockaddr*)&ClientAddress, &caSize)) == INVALID_SOCKET)
		{
			Con_color(12);
			cout << "Invalid accept socket, connection failed\n";
			closesocket(ServerSocket);
			WSACleanup();
			Sleep(3000);
			return -1;
		}

// show user, ip and port connected user
		Con_color(11);
		cout << "User Num."<< ClientCount+1 << " successfully connection with " <<	inet_ntoa(ClientAddress.sin_addr) << ":" << ntohs(ClientAddress.sin_port) << endl;
		Con_color(15);
//		ShowInfoClient();
//--------------------------------------------------------------------------------
		pNewConnectSocket[ClientCount] = NewClientSocket;
		send(pNewConnectSocket[ClientCount], messegeClient, strlen(messegeClient), NULL);
		ClientCount++;
//--------------------------------------------------------------------------------

		hThreadSock = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ThreadSendReceiveFunc, (LPVOID)(ClientCount-1), 0, 0);
		if (hThreadSock == NULL)
		{
			Con_color(12);
			cout << "Error thread " << GetLastError() << endl;
			Sleep(3000);
			return -1;
		}
	}
	
	closesocket(NewClientSocket);

	return 0;
}

DWORD WINAPI ThreadMessAll( void *pV )
{
	setlocale(0, "");
	SetConsoleOutputCP(1251);
	SetConsoleOutputCP(1251);

	for (int i = 0; i <= ClientCount; i++)
		send(pNewConnectSocket[i], buff, nData, 0);

	return 0;
}

void ShowInfoClient()
{
	struct hostent *hptr = {0};
	char ip[50];
	int port;

	inet_ntop(AF_INET, &ServAddress.sin_addr.s_addr, ip, 50);
	getsockname(NewClientSocket, (struct sockaddr *)&ServAddress, &saSize);
	hptr = gethostbyaddr((char*)&ServAddress.sin_addr.s_addr, 4, AF_INET);
	port =  ntohs(ServAddress.sin_port);

	Con_color(14);
	cout << "User: " << hptr->h_name;
	cout << " connected from " << ip << ":" << port << endl;
	Con_color(15);

//	cout << "Successfully connection with " <<	inet_ntoa(ClientAddress.sin_addr) << ":" << ntohs(ClientAddress.sin_port) << endl;
}
