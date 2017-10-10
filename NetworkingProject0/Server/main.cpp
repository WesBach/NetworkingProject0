#define UNICODE
#define WIN_32_CHAT_APP_SERVER

//#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Buffer.h"

#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "8899"
#define DEFAULT_BUFFER_LENGTH 512

//Header struct for packet length and message id
struct Header {
public:
	//[packet_length][message_id]
	int packet_length;			//in bytes
	int message_id;				//who it came from
};

//Protocols
void sendMessage(Header &theHeader,
	int &roomNameLength,
	std::string &roomName,
	int &messageLength,
	std::string &message
);
void receiveMessage(Header &theHeader,
	int &senderNameLength,
	std::string &senderName,
	int &messageLength,
	std::string &message,
	int &roomNameLength,
	std::string &roomName
);
void joinRoom(Header &theHeader, int &roomNameLength, std::string &roomName);
void leaveRoom(Header &theHeader, int &roomNameLength, std::string &roomName);


int main()
{
	//create the buffer object on the heap
	Buffer *messageBuffer;
	messageBuffer = new Buffer(DEFAULT_BUFFER_LENGTH);

	//create a socket for the server and an fd_set with the number of client sockets and an array of those sockets
	SOCKET ListenSocket;
	SOCKET AcceptSocket;
	//
	fd_set readSet;
	fd_set writeSet;
	FD_ZERO(&readSet);

	WSADATA wsaData;

	struct addrinfo* result = 0;
	struct addrinfo addressInfo;
	int iResult = 0;

	//create a socket for the server with the port 8899
	ZeroMemory(&addressInfo, sizeof(addressInfo));
	addressInfo.ai_family = AF_INET;
	addressInfo.ai_socktype = SOCK_STREAM;
	addressInfo.ai_protocol = IPPROTO_TCP;
	addressInfo.ai_flags = AI_PASSIVE;

	// Socket()
	ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Bind()
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &addressInfo, &result);
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind() failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Listen()
	if (listen(ListenSocket, 5)) {
		printf("listen() failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}


	// Accept the connection.
	AcceptSocket = accept(ListenSocket, NULL, NULL);
	if (AcceptSocket == INVALID_SOCKET) {
		wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	else
		wprintf(L"Client connected.\n");

	closesocket(ListenSocket);
	WSACleanup();

}


//TO DO: Fill in the protocol functions 
void sendMessage(Header &theHeader, int &roomNameLength, std::string &roomName, int &messageLength, std::string &message)
{
}

void receiveMessage(Header & theHeader, int & senderNameLength, std::string & senderName, int & messageLength, std::string & message, int & roomNameLength, std::string & roomName)
{
}

void joinRoom(Header &theHeader, int &roomNameLength, std::string &roomName)
{
}

void leaveRoom(Header &theHeader, int &roomNameLength, std::string &roomName)
{
}
