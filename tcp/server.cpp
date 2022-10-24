#include <iostream>

// используется 2-я версия winsock
#include <winsock2.h>

// linker подключит библиотеку в список библиотечных зависимостей
#pragma comment (lib, "Ws2_32.lib")
  

using namespace std;

#define	DEFAULT_PORT 5000

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


	// 2) <описываем адрес сокета>
	sockaddr_in sin;

	sin.sin_family = AF_INET; // задали семейство IP (IPv4)

	// htons - преобразовывает данные локального хоста в сетевой формат
	sin.sin_port = htons(DEFAULT_PORT); // задали порт

	// s_addr ~ S_un.S_addr
	// INADDDR_ANY - сокет работает с любым IP-адресом
	sin.sin_addr.s_addr = INADDR_ANY; // задали IP, с которым будет работать сокет.


	// 3) <привязываем адрес к сокету>
	bind(s, (sockaddr*) &sin, sizeof(sin));

	listen(s, SOMAXCONN);

	sockaddr addr;
	int len = sizeof(addr);
	accept(s, &addr, &len);



	// n) закрываем сокет
	closesocket(s);



	cleanUp();

	return 0;
}
