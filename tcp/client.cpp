#include <iostream>
#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

#define	SERVER_PORT 5000


int cleanUp()
{
	if (WSACleanup() == SOCKET_ERROR) {
		cout << "WSACleanup error: " << WSAGetLastError() << '\n';
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


	SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (listen_socket == INVALID_SOCKET) {
		cout << "socket error: " << WSAGetLastError() << '\n';
		closesocket(listen_socket);
		cleanUp();
	}

	sockaddr_in s_in;

	s_in.sin_family = AF_INET;
	s_in.sin_port = htons(SERVER_PORT);
	s_in.sin_addr.s_addr = INADDR_ANY;

	iResult = bind(listen_socket, (sockaddr*) &s_in, sizeof(s_in));

	if (iResult == INVALID_SOCKET) {
		cout << "bind error: " << WSAGetLastError() << '\n';
		closesocket(listen_socket);
		cleanUp();
	}

	iResult = listen(listen_socket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		cout << "listen error: " << WSAGetLastError() << '\n';
		closesocket(listen_socket);
		cleanUp();
	}

	int from_len;
	sockaddr_in from_s_in;
	from_len = sizeof(from_s_in);

	SOCKET new_socket;

	while (true) {
		new_socket = accept(listen_socket, (sockaddr*)&from_s_in, &from_len);
		cout << "New connection\n";
		closesocket(new_socket);
	}

	closesocket(listen_socket);
	cleanUp();

	return 0;
}
