#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

#define	SERVER_PORT 5000
#define	CLIENT_PORT 1234
#define BUF_LEN 64


int cleanUp()
{

	if (WSACleanup() == SOCKET_ERROR) {
		cout << "WSACleanup() error " << WSAGetLastError() << '\n';
		return 1;
	} else {
		return 0;
	}
}


int main()
{
	int iResult;

	WSAData wsaData;

	iResult = WSAStartup(0x0202, &wsaData);

	if (iResult) {
		cout << "WSAStartup error: " << WSAGetLastError() << '\n';
		return cleanUp();
	}


	SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);

	if (s == INVALID_SOCKET) {
		cout << "socket error: " << WSAGetLastError() << '\n';
		closesocket(s);
		return cleanUp();
	}

	sockaddr_in s_in;
	ZeroMemory(&s_in, sizeof(s_in));

	s_in.sin_family = AF_INET;
	s_in.sin_port = htons(CLIENT_PORT);
	s_in.sin_addr.s_addr = INADDR_ANY;

	iResult = bind(s, (sockaddr*)&s_in, sizeof(s_in));
	if (iResult == INVALID_SOCKET) {
		cout << "bind error: " << WSAGetLastError() << '\n';
		closesocket(s);
		return cleanUp();
	}

	// адрес сервера
	sockaddr_in remote_addr;
	ZeroMemory(&remote_addr, sizeof(remote_addr));

	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(SERVER_PORT);
	remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


	string msg = "echo message";

	int remote_len;
	char buf[BUF_LEN] = {0};


	while (true) {
		cin >> msg;
		sendto(s, (char*) &msg[0], msg.size(), 0, (sockaddr*)&remote_addr, sizeof(remote_addr));

		remote_len = sizeof(remote_addr);

		int buf_size = recvfrom(s, &buf[0], sizeof(buf) - 1, 0, (sockaddr*)&remote_addr, &remote_len);

		if (buf_size == SOCKET_ERROR) {
			cout << "recvfrom error" << WSAGetLastError() << '\n';
		}

		cout << "NEW DATAGRAM\n";
		buf[buf_size] = '\0';
		cout << buf << '\n';
	}

	closesocket(s);
	return cleanUp();
}
