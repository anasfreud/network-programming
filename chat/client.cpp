#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <string>
#include <vector>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

#define	SERVER_PORT 5000

struct Message {
	char nickname[255] = {};
	char message[255] = {};
	bool isPrivate = false;
	char to[255] = {};

	void setNickname(string nick) {
		strcpy_s(nickname, nick.c_str());
	}
	void setMessage(string msg) {
		strcpy_s(message, msg.c_str());
	}
	void setTo(string toUser) {
		strcpy_s(to, toUser.c_str());
	}
	string getNickname() {
		return nickname;
	}
	string getMessage() {
		return message;
	}
	string getTo() {
		return to;
	}
};


DWORD WINAPI recieve(LPVOID client_socket) {
	SOCKET s = ((SOCKET*)client_socket)[0];
	Message resp;
	int from_len = recv(s, (char*)&resp, sizeof(resp), 0);

	while (from_len != SOCKET_ERROR) {
		if (resp.isPrivate) {
			cout << "<private> ";
		}
		cout << resp.getNickname() << ": " << resp.getMessage() << '\n';
		from_len = recv(s, (char*)&resp, sizeof(resp), 0);
	}
	return 0;
}


int cleanUp()
{
	if (WSACleanup() == SOCKET_ERROR) {
		cout << "WSACleanup() error " << WSAGetLastError() << '\n';
		exit(-1);
	}
}

vector<string> split(string str) {
	vector<string> v;
	int index = 0;
	while (index < str.length()) {
		string substr = "";
		while (str[index] != ' ' && index < str.length()) {
			substr += str[index];
			index++;
		}
		index++;
		v.push_back(substr);
	}
	return v;
}


int main()
{
	int iResult;

	WSAData wsaData;

	iResult = WSAStartup(0x0202, &wsaData);

	if (iResult) {
		cout << "WSAStartup error: " << WSAGetLastError() << '\n';
		cleanUp();
		return 0;
	}


	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

	if (s == INVALID_SOCKET) {
		cout << "socket error: " << WSAGetLastError() << '\n';
		closesocket(s);
		cleanUp();
		return 0;
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
		cleanUp();
		return 0;
	}

	cout << "Welcome to chat! Enter your username: \n";
	string username;
	getline(cin, username);

	cout << "\n - To quit the application, type /quit\n";
	cout << " - To send private message, type /private [username] [message]\n";
	cout << " - To send public message, type your message and press ENTER\n\n";


	DWORD thID;
	CreateThread(NULL, NULL, recieve, &s, NULL, &thID);

	Message m;
	m.setNickname(username);
	string msg = "";

	send(s, (char*)&m, sizeof(m), 0);

	while (true) {

		getline(cin, msg);
		if (msg == "") {
			continue;
		}
		vector<string> words = split(msg);

		if (words[0][0] == '/') {
			if (words[0] == "/quit") {
				break;
			} else if (words[0] == "/private") {
				m.isPrivate = true;
				m.setTo(words[1]);
				m.setMessage(msg.erase(0, words[0].length() + words[1].length() + 2));
				send(s, (char*)&m, sizeof(m), 0);
			} else {
				cout << "Unknown command\n";
			}
		} else {
			m.isPrivate = false;
			m.setMessage(msg);
			send(s, (char*)&m, sizeof(m), 0);
		}
	}

	closesocket(s);
	cleanUp();
	return 0;
}
