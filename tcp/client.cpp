#include <iostream>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

// используется 2-я версия winsock
#include <winsock2.h>

// linker подключит библиотеку в список библиотечных зависимостей
#pragma comment (lib, "Ws2_32.lib")


using namespace std;

#define	SERVER_PORT 5000


int cleanUp()
{
	// освобождение ресурсов, выделенных Winsock

	if (WSACleanup() == SOCKET_ERROR) {
		cout << "WSACleanup() error " << WSAGetLastError() << '\n';
		return 0;
	}
	return 1;
}

int main()
{
	int iResult;

	// 0) <регистрация приложения в библиотеке>
	WSAData wsaData;

	if (WSAStartup(0x0202, &wsaData)) { //0x0202 - версия 2.2 интерфейса Winsock
		cout << "WSAStartup() error " << WSAGetLastError() << '\n';
		cleanUp();
	}


	// 1) <создаем сокет>
	// AF_INET - протокол IPv4, 
	// SOCK_STREAM - потоковый режим соединения, 
	// 0 - протокол соединения по умолчанию (TCP)

	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		cout << "socket() error " << WSAGetLastError() << '\n';
		cleanUp();
	}


	// 2) <описываем адрес удаленного сокета>
	sockaddr_in remote_addr;

	ZeroMemory(&remote_addr, sizeof(remote_addr));

	remote_addr.sin_family = AF_INET; // задали семейство IP (IPv4)

	// htons - преобразовывает данные локального хоста в сетевой формат
	remote_addr.sin_port = htons(SERVER_PORT); // задали порт сервера

	// s_addr ~ S_un.S_addr
	remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // задали IP сервера


	// 3) <подключаем сокет к удаленному сокету сервера>
	iResult = connect(s, (sockaddr*) &remote_addr, sizeof(remote_addr));

	if (iResult == SOCKET_ERROR) {
		cout << "Server connection error: " << WSAGetLastError() << '\n';
		closesocket(s);
		cleanUp();
	}






	// n) закрываем сокет
	closesocket(s);


	cleanUp();

	return 0;
}


/*
* в клиенте надо привязать к сокету с помощью connect sockaddr сервера
* bind происходит автоматически, с сокетом связывается случайный порт
* 
* 
* для TCP
*	используем connect, запрашивая соединение с сервером, создавая подключение
* для UDP
*	используем bind, привязывая определенный порт к сокету. подключение не создается	
*/
