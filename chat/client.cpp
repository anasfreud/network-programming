#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

#define	SERVER_PORT 5000
#define BUF_LEN 64

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

// прием сообщений
DWORD WINAPI recieveMessages(LPVOID socket) {
	SOCKET s = ((SOCKET*)socket)[0];
	Message m;
	int from_len = 1;
	while (from_len != SOCKET_ERROR) {
		int from_len = recv(s, (char*)&m, sizeof(m), 0);
		cout << m.getNickname() + ": " + m.getMessage() + '\n';
	}
	return 0;
}


int cleanUp()
{

	if (WSACleanup() == SOCKET_ERROR) {
		cout << "WSACleanup() error " << WSAGetLastError() << '\n';
		return 1;
	}
	else {
		return 0;
	}
}

bool isRecievingMessages = false;

int main()
{
	cout << "Welcome to the local chat!\n";
	cout << "Enter your nickname\n";
	string nickname;
	cin >> nickname;

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

	iResult = connect(s, (sockaddr*)&remote_addr, sizeof(remote_addr));

	if (iResult == SOCKET_ERROR) {
		cout << "Server connection error: " << WSAGetLastError() << '\n';
		closesocket(s);
		return cleanUp();
	}

	cout << "Succesfully connected to the server. You can chat now! :3\n";
	
	while (true) {
		cout << "Avalible commands:\n";
		cout << "/general - enter general chat;\n";
		cout << "/private - send private message;\n";
		cout << "/quit - close application;\n";

		string cmd;
		cin >> cmd;
		if (cmd == "/general") {

			cout << "Enter /leave to leave general chat;\n";

			Message m;
			m.setNickname(nickname);

			DWORD thID;
			CreateThread(NULL, NULL, recieveMessages, &s, NULL, &thID);

			string msg;
			getline(cin, msg);
			while (getline(cin, msg)) {
				if (msg == "/leave") {
					break;
				}
				m.setMessage(msg);
				send(s, (char*)&m, sizeof(m), 0);
			}
		} else if (cmd == "/private") {
			cout << "Not implemented yet :(\n";
		} else if (cmd == "/quit") {
			shutdown(s, SD_BOTH);
			closesocket(s);
			//return cleanUp();
		}

	}
	
	

	closesocket(s);
	
	return cleanUp();
}
