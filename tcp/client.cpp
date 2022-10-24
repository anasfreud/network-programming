#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

#define	SERVER_PORT 5000


int cleanUp()
{

	if (WSACleanup() == SOCKET_ERROR) {
		cout << "WSACleanup() error " << WSAGetLastError() << '\n';
		return 0;
	} else {
		return 1;
	}
}


int main()
{
	int iResult;

	WSAData wsaData;

	iResult = WSAStartup(0x0202, &wsaData);

	if (iResult) {
		cout << "WSAStartup error: " << WSAGetLastError() << '\n';
		cleanUp();
	}


	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

	if (s == INVALID_SOCKET) {
		cout << "socket error: " << WSAGetLastError() << '\n';
		closesocket(s);
		cleanUp();
	}


	sockaddr_in remote_addr;

	ZeroMemory(&remote_addr, sizeof(remote_addr));

	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(SERVER_PORT);
	remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	iResult = connect(s, (sockaddr*) &remote_addr, sizeof(remote_addr));

	if (iResult == SOCKET_ERROR) {
		cout << "Server connection error: " << WSAGetLastError() << '\n';
		closesocket(s);
		cleanUp();
	}

	int from_len;
	char buf[64] = { 0 };

	string msg;

	do {
		from_len = recv(s, (char*)buf, 64, 0);

	} while (msg != "Bye");





	closesocket(s);
	cleanUp();

	return 0;
}
