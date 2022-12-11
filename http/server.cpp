#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <sstream>
#include <winsock2.h>
#include <string>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

#define	SERVER_PORT 80
#define BUF_SIZE 1024


int cleanUp()
{
	if (WSACleanup() == SOCKET_ERROR) {
		cout << "WSACleanup error: " << WSAGetLastError() << '\n';
		return 1;
	}
	else {
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


	SOCKET from_socket = INVALID_SOCKET;
	char buf[BUF_SIZE];

	cout << "Server has started on port " << SERVER_PORT << '\n';

	while (true) {
		from_socket = accept(listen_socket, NULL, NULL);

		if (from_socket == INVALID_SOCKET) {
			cerr << "accept error: " << WSAGetLastError();
			closesocket(from_socket);
			return cleanUp();
		}

		stringstream response;
		stringstream body;

		int from_len = recv(from_socket, (char*)&buf, BUF_SIZE, 0);

		if (from_len == SOCKET_ERROR) {
			cerr << "recv error: " << WSAGetLastError();
			closesocket(from_socket);
			return cleanUp();
		} else if (from_len == 0) {
			cerr << "Connection closed\n";
		} else if (from_len > 0) {
			buf[from_len] = 0;
			body << "<title>Test C++ HTTP Server</title>\n"
				<< "<h1>Hello world!</h1>\n"
				<< "<pre>" << buf << "</pre>\n";

			response << "HTTP/1.1 200 OK\r\n"
				<< "Version: HTTP/1.1\r\n"
				<< "Content-Type: text/html; charset=utf-8\r\n"
				<< "Content-Length: " << body.str().length() << "\r\n"
				<< "\r\n\r\n" << body.str();

			iResult = send(from_socket, response.str().c_str(), response.str().length(), 0);
			closesocket(from_socket);
		}
	}

	closesocket(listen_socket);
	WSACleanup();

	return 0;

}
