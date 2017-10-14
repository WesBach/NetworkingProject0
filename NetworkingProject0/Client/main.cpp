#include "Buffer.h"
#include <string>
#include <iostream>
#include <conio.h>

//#include <Windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "5000"
#define DEFAULT_BUFFER_LENGTH 512

//TO DO: Client side connection
int main(int argc, char** argv) {
	
	//if (argc != 2) {
	//	printf("usage: %s server-name\n", argv[0]);
	//	//return 1;
	//}

	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//TODO: Replace the argv[1] with something from the program.
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}


	//Connecting to server?
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket() failed with error: %d\n", iResult);
			WSACleanup();
			return 1;
		}

		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}

		break;
	}

	freeaddrinfo(result);

	//Check if the Connected socket is valid
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server");
		WSACleanup();
		return 1;
	}

	//Taking in user's input and quiting when detecting the 'q' (infinite loop)
	while (true)
	{
		if (_kbhit()) 
		{
			char ch = _getch();
			std::cout << ch;

			//SUDO CODE - Take in what the user has typed in and send it to the server.
			//if (ENTER IS PRESSED)
			//{
				////send the Server some sort of infomation form the client (this should be the Join/Leave/Send stuff
				//iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
				//if (iResult == SOCKET_ERROR) {
				//	printf("socket() failed with error: %d\n", iResult);
				//	closesocket(ConnectSocket);
				//	WSACleanup();
				//	return 1;
				//}
			//}

			//if (ch == 'q')
			//{
			//	std::cout << "QUIT" << std::endl;

				//this should be called when you want to exit the program NOT NEEDED NOW
				//iResult = shutdown(ConnectSocket, SD_SEND);
				//if (iResult == SOCKET_ERROR) {
				//	printf("shutdown() failed with error: %d\n", iResult);
				//	closesocket(ConnectSocket);
				//	WSACleanup();
				//	return 1;
				//}

				//closesocket(ConnectSocket);
				//WSACleanup();
			//	break;
			//}
		}
	}
}