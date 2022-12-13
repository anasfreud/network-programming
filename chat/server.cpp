#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <string>
#include <vector>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

#define	SERVER_PORT 5000
#define BUF_SIZE 64

struct Message {
	char nickname[255];
	char message[255];

	void setNickname(string nick) {
		strcpy_s(nickname, nick.c_str());
	}
	void setMessage(string msg) {
		strcpy_s(message, msg.c_str());
	}
	string getNickname() {
		return nickname;
	}
	string getMessage() {
		return message;
	}
};


vector<SOCKET> clientSockets;

int clients = 0;

void printUsers() {
	cout << "Clients online: " << clients << '\n';
}

DWORD WINAPI connectToClient(LPVOID client_socket) {

	// послать всем клиентам сообщение о том что клиент присоединился


	SOCKET from_socket = ((SOCKET*)client_socket)[0];
	int from_len = 1;

	Message m;

	while (from_len) {
		
		from_len = recv(from_socket, (char*)&m, sizeof(m), 0);
		cout << "recieved" << m.getMessage() << endl;
		for (int i = 0; i < clientSockets.size(); i++) {
			cout << "sending\n";
			send(clientSockets[i], (char*)&m, sizeof(m), 0);
		}
	}

	cout << "Connection closed\n";
	shutdown(from_socket, SD_BOTH);
	closesocket(from_socket);
	clients--;
	printUsers();
	return 0;
}

int cleanUp()
{
	if (WSACleanup() == SOCKET_ERROR) {
		cout << "WSACleanup error: " << WSAGetLastError() << '\n';
	}
	return -1;
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

	cout << "Server has started on port " << SERVER_PORT << '\n';

	while (true) {
		int from_len = sizeof(from_s_in);
		from_socket = accept(listen_socket, (sockaddr*)&from_s_in, &from_len);

		cout << "New connection with " << inet_ntoa(from_s_in.sin_addr) << '\n';
		clients++;
		clientSockets.push_back(from_socket);

		printUsers();

		DWORD thID;
		CreateThread(NULL, NULL, connectToClient, &from_socket, NULL, &thID);
	}

	closesocket(listen_socket);
	return cleanUp();

}
