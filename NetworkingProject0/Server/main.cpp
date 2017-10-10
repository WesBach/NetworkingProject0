#define UNICODE
#define WIN_32_CHAT_APP_SERVER

#include <Windows.h>
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
	////create the buffer object on the heap
	Buffer *messageBuffer;
	messageBuffer = new Buffer(DEFAULT_BUFFER_LENGTH);

	//create a socket for the server and an fd_set with the number of client sockets and an array of those sockets
	SOCKET listenSocket;
	fd_set clientSockets;
	//initialize the fd_set
	FD_ZERO(&clientSockets);

	WSADATA wsaData;

	struct addrinfo* result = 0;
	struct addrinfo addressInfo;

	//create a socket for the server with the port 8899
	ZeroMemory(&addressInfo, sizeof(addressInfo));
	addressInfo.ai_family = AF_INET;
	addressInfo.ai_socktype = SOCK_STREAM;
	addressInfo.ai_protocol = IPPROTO_TCP;
	addressInfo.ai_flags = AI_PASSIVE;

	// Setup the TCP listening socket
	if (bind(listenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

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
