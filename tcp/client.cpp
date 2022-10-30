#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

#define	SERVER_PORT 5000
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


	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

	if (s == INVALID_SOCKET) {
		cout << "socket error: " << WSAGetLastError() << '\n';
		closesocket(s);
		return cleanUp();
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
		return cleanUp();
	}

	char buf[BUF_LEN] = { 0 };
	string msg;

	cout << "Connected to server\n";

	do {
		cout << "Waiting for respond...\n";
		int from_len = recv(s, (char*)buf, BUF_LEN, 0);
		cout << "Server responded: ";
		for (int i = 0; i < from_len; i++) {
			cout << buf[i];
		}

		cout << "\nPlease, enter your request\n";

		getline(cin, msg);

		send(s, (char*) msg.c_str(), msg.size(), 0);

	} while (msg != "stop");


	closesocket(s);
	return cleanUp();
}
