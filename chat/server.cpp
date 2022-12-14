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

struct client {
	string nickname;
	SOCKET socket;

	client(SOCKET s, string nick) {
		socket = s;
		nickname = nick;
	}

	void sendMessage(Message message) {
		send(socket, (char*)&message, sizeof(message), 0);
	}
};
vector<client> clients;

void printUsers() {

	cout << "Clients online: " << '\n';
	for (int i = 0; i < clients.size(); i++) {
		cout << clients[i].nickname << '\n';
	}
}


DWORD WINAPI connectToClient(LPVOID client_socket) {

	SOCKET from_socket = ((SOCKET*)client_socket)[0];
	Message resp;
	string clientNickname;


	int	from_len = recv(from_socket, (char*)&resp, sizeof(resp), 0);

	if (from_socket != SOCKET_ERROR) {
		clientNickname = resp.getNickname();
		resp.setNickname("SERVER");
		resp.setMessage(clientNickname + " entered this chat");

		clients.push_back(client{ from_socket, clientNickname });

		for (int i = 0; i < clients.size(); i++) {
			if (clients[i].nickname != clientNickname) {
				clients[i].sendMessage(resp);
			}
		}
	}

	printUsers();
	
	from_len = recv(from_socket, (char*)&resp, sizeof(resp), 0);

	while (from_len != SOCKET_ERROR) {

		cout << "Recieved: " << resp.getMessage() << " from " << resp.getNickname() << '\n';

		if (resp.isPrivate) {
			cout << "Private\n";
			for (int i = 0; i < clients.size(); i++) {
				if (clients[i].nickname == resp.getTo()) {
					clients[i].sendMessage(resp);
					break;
				}
			}
		}
		else {
			for (int i = 0; i < clients.size(); i++) {
				if (clients[i].nickname != resp.getNickname()) {
					clients[i].sendMessage(resp);
				}
			}
		}
		from_len = recv(from_socket, (char*)&resp, sizeof(resp), 0);
	}

	cout << "Connection closed\n";

	resp.setNickname("SERVER");
	resp.setMessage(clientNickname + " left this chat");
	for (int i = 0; i < clients.size(); i++) {
		if (from_socket == clients[i].socket) {
			clients.erase(clients.begin() + i);
			i--;
		}
		else {
			clients[i].sendMessage(resp);
		}
	}

	closesocket(from_socket);
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
		from_socket = accept(listen_socket, (sockaddr*)&from_s_in, &from_len); //!

		cout << "New connection with " << inet_ntoa(from_s_in.sin_addr) << '\n';

		DWORD thID;
		CreateThread(NULL, NULL, connectToClient, &from_socket, NULL, &thID);
	}

	closesocket(listen_socket);
	return cleanUp();

}
