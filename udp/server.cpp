#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

#define	SERVER_PORT 5000
#define BUF_SIZE 64


int cleanUp()
{
	if (WSACleanup() == SOCKET_ERROR) {
		cout << "WSACleanup error: " << WSAGetLastError() << '\n';
		return 0;
	}
	else {
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
		return cleanUp();
	}


	SOCKET listen_socket = socket(AF_INET, SOCK_DGRAM, 0);

	if (listen_socket == INVALID_SOCKET) {
		cout << "socket error: " << WSAGetLastError() << '\n';
		closesocket(listen_socket);
		return cleanUp();
	}

	sockaddr_in s_in;
	ZeroMemory(&s_in, sizeof(s_in));

	s_in.sin_family = AF_INET;
	s_in.sin_port = htons(SERVER_PORT);
	s_in.sin_addr.s_addr = INADDR_ANY;

	iResult = bind(listen_socket, (sockaddr*)&s_in, sizeof(s_in));

	if (iResult == INVALID_SOCKET) {
		cout << "bind error: " << WSAGetLastError() << '\n';
		closesocket(listen_socket);
		return cleanUp();
	}


	SOCKET from_socket;
	sockaddr_in from_s_in;
	int from_len;

	char buf[BUF_SIZE];

	string msg;
	
	while (true) {

		from_len = sizeof(from_s_in);

		int buf_size = recvfrom(listen_socket, &buf[0], sizeof(buf) - 1, 0, (sockaddr*)&from_s_in, &from_len);
		
		if (buf_size == SOCKET_ERROR) {
			cout << "recvfrom error" << WSAGetLastError() << '\n';
		}

		cout << "NEW DATAGRAM\n";
		buf[buf_size] = '\0';
		cout << buf << '\n';

		sendto(listen_socket, &buf[0], buf_size, 0, (sockaddr*)&from_s_in, sizeof(from_s_in));

	}

	closesocket(listen_socket);
	return cleanUp();

}
