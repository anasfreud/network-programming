#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

//#define request "GET /user HTTP/1.0 \r\nHost: www.kubsu.ru \r\n\r\n"
#define	SERVER_PORT 80

#define BUF_LEN 4096


int cleanUp()
{
	if (WSACleanup() == SOCKET_ERROR) {
		cout << "WSACleanup() error " << WSAGetLastError() << '\n';
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


	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

	if (s == INVALID_SOCKET) {
		cout << "socket error: " << WSAGetLastError() << '\n';
		closesocket(s);
		return cleanUp();
	}


	hostent* h = gethostbyname("www.json.org");
	if (h == NULL) {
		cout << "gethostbyname error: " << WSAGetLastError() << '\n';
		closesocket(s);
		return cleanUp();
	}


	sockaddr_in remote_addr;
	ZeroMemory(&remote_addr, sizeof(remote_addr));

	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(SERVER_PORT);
	remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//((unsigned long*)&remote_addr.sin_addr)[0] = ((unsigned long**)h->h_addr_list)[0][0];



	iResult = connect(s, (sockaddr*)&remote_addr, sizeof(remote_addr));

	if (iResult == SOCKET_ERROR) {
		cout << "Server connection error: " << WSAGetLastError() << '\n';
		closesocket(s);
		return cleanUp();
	}

	cout << "Connected to server\n";
	
	// посылаем GET-запрос
	string request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";

	iResult = send(s, (char*)request.c_str(), sizeof(request), 0);

	if (iResult == SOCKET_ERROR) {
		cout << "Send error: " << WSAGetLastError() << '\n';
		closesocket(s);
		return cleanUp();
	}

	char buf[BUF_LEN] = { 0 };
	int len = 0;

	do {
		len = recv(s, (char*)buf, BUF_LEN, 0);
		if (len == SOCKET_ERROR) {
			cout << "recv error: " << WSAGetLastError() << '\n';
			closesocket(s);
			return cleanUp();
		}
		for (int i = 0; i < len; i++) {
			cout << buf[i];
		}
		

	} while (len != 0);


	closesocket(s);
	
	if (WSACleanup() == SOCKET_ERROR) {
		cout << "WSACleanup() error " << WSAGetLastError() << '\n';
		return -1;
	}
	return 0;
}
