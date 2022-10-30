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


	SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);

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

	iResult = listen(listen_socket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		cout << "listen error: " << WSAGetLastError() << '\n';
		closesocket(listen_socket);
		return cleanUp();
	}

	SOCKET from_socket;
	sockaddr_in from_s_in;

	char buf[BUF_SIZE];

	cout << "Server has started on port " << SERVER_PORT << '\n';
	string msg = "Hello from server!";

	while (true) {
		int from_len = sizeof(from_s_in);
		from_socket = accept(listen_socket, (sockaddr*)&from_s_in, &from_len);

		cout << "New connection with " << inet_ntoa(from_s_in.sin_addr) << '\n';

		while (true) {
			send(from_socket, msg.c_str(), msg.size(), 0);
			cout << "Waiting for request...\n";

			from_len = recv(from_socket, (char*)&buf, BUF_SIZE, 0);

			string respond(buf, 4);
			if (respond == "stop") {
				break;
			}

			cout << "Client requested: ";

			for (int i = 0; i < from_len; i++) {
				cout << buf[i];
			}
			cout << '\n';


			cout << "Please, enter your respond\n";
			getline(cin, msg);
		}

		cout << "Connection closed\n";
		closesocket(from_socket);
	}

	closesocket(listen_socket);
	return cleanUp();

}
