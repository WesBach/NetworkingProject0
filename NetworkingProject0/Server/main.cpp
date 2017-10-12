#define UNICODE
#define WIN_32_CHAT_APP_SERVER

//#include <Windows.h>   freaks out if i include this
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Buffer.h"

#include <iostream>

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT "8899"
#define DEFAULT_BUFFER_LENGTH 1024
//socket info structure to store all the individual socket information
struct SocketInfo {
	int ID;
	Buffer buffer[DEFAULT_BUFFER_LENGTH];
	SOCKET socket;
	DWORD bytesSEND;
	DWORD bytesRECV;
};

//Header struct for packet length and message id
struct Header {
public:
	//[packet_length][message_id]
	int packet_length;			//in bytes
	int message_id;				//who it came from
};

//read packet function
//void readPacket(SOCKET socket);
//get socket function for populating the socket info and pushing back onto
void addSocketInformation(SOCKET socket);
void freeSocketInformation(int Index);
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

//sockets
std::vector<SocketInfo> g_theSockets;
int g_IDCounter = 0;

int main()
{
	SOCKET ListenSocket;
	SOCKET AcceptSocket;
	fd_set readSet;
	fd_set writeSet;
	FD_ZERO(&readSet);
	WSADATA wsaData;
	struct addrinfo* result = 0;
	struct addrinfo addressInfo;
	int iResult = 0;
	int totalSocketsInSet = 0;
	DWORD flags;
	DWORD RecvBytes;
	DWORD SendBytes;

	//create a socket for the server with the port 8899
	ZeroMemory(&addressInfo, sizeof(addressInfo));
	addressInfo.ai_family = AF_INET;
	addressInfo.ai_socktype = SOCK_STREAM;
	addressInfo.ai_protocol = IPPROTO_TCP;
	addressInfo.ai_flags = AI_PASSIVE;


	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	// Socket()
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

	//// Accept the connection.
	//AcceptSocket = accept(ListenSocket, NULL, NULL);
	//if (AcceptSocket == INVALID_SOCKET) {
	//	wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
	//	closesocket(ListenSocket);
	//	WSACleanup();
	//	return 1;
	//}

	ULONG nonBlock = 1;
	if (ioctlsocket(ListenSocket, FIONBIO, &nonBlock) == SOCKET_ERROR)
	{
		printf("ioctlsocket() failed with error %d\n", WSAGetLastError());
		return 1;
	}

	while (true) {

		//zero the sets
		FD_ZERO(&readSet);
		FD_ZERO(&writeSet);

		//listen for connections on listen socket
		FD_SET(ListenSocket, &readSet);

		// Set Read and Write notification for each socket based on the
		// current state of the buffer.  If there is data remaining in the
		// buffer then set the Write set otherwise the Read set
		for (int i = 0; i < g_theSockets.size(); i++)
		{
			if (g_theSockets[i].bytesRECV > g_theSockets[i].bytesSEND)
				FD_SET(g_theSockets[i].socket, &writeSet);
			else
				FD_SET(g_theSockets[i].socket, &readSet);
		}

		if ((totalSocketsInSet = select(0, &readSet, &writeSet, NULL, NULL)) == SOCKET_ERROR)
		{
			printf("select() returned with error %d\n", WSAGetLastError());
			return 1;
		}

		// Check for arriving connections on the listening socket.
		if (FD_ISSET(ListenSocket, &readSet))
		{
			totalSocketsInSet--;
			if ((AcceptSocket = accept(ListenSocket, NULL, NULL)) != INVALID_SOCKET)
			{
				// Set the accepted socket to non-blocking mode
				nonBlock = 1;
				if (ioctlsocket(AcceptSocket, FIONBIO, &nonBlock) == SOCKET_ERROR)
				{
					printf("ioctlsocket(FIONBIO) failed with error %d\n", WSAGetLastError());
					return 1;
				}

				//add the socket info to the socket vector
				addSocketInformation(AcceptSocket);
			}
			else
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					printf("accept() failed with error %d\n", WSAGetLastError());
					return 1;
				}
			}
		}

		// Check each socket for Read and Write notification until the number
		// of sockets in Total is satisfied
		for (int i = 0; totalSocketsInSet > 0 && i < g_theSockets.size(); i++)
		{
			SocketInfo socketInfo = g_theSockets[i];

			// If the ReadSet is marked for this socket then this means data
			// is available to be read on the socket
			if (FD_ISSET(socketInfo.socket, &readSet))
			{
				totalSocketsInSet--;
				//read the buffer data
				/*socketInfo.dataBuf.buf = socketInfo.buffer;
				socketInfo.dataBuf.len = DATA_BUFSIZE;*/

				flags = 0;
				//To Do: replace this functionality with reading the information with the buffer class we created
				//or add it maybe?
				if (recv(socketInfo.socket, (char*)socketInfo.buffer, socketInfo.buffer->GetBufferLength(),flags) == SOCKET_ERROR)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						printf("WSARecv() failed with error %d\n", WSAGetLastError());
						freeSocketInformation(i);
					}
					continue;
				}
			}//if (FD_ISSET(socketInfo.socket, &readSet))


			// If the WriteSet is marked on this socket then this means the internal
			// data buffers are available for more data
			if (FD_ISSET(socketInfo.socket, &writeSet))
			{
				totalSocketsInSet--;

				if (send(socketInfo.socket, (char*)socketInfo.buffer, socketInfo.buffer->GetBufferLength() , flags) == SOCKET_ERROR)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						printf("WSASend() failed with error %d\n", WSAGetLastError());
						freeSocketInformation(i);
					}

					continue;
				}
			}//if (FD_ISSET(socketInfo.socket, &writeSet))
		}//for (int i = 0; totalSocketsInSet > 0 && i < g_theSockets.size(); i++)	
	}//while (true) {

	//clean up
	closesocket(ListenSocket);
	WSACleanup();
}


void ReadPacket(SOCKET socket)
{
	//int result = recv(socket, &buffer, buflen, flags);
	//if (result = INVALID_SOCKET)
	//{
	//	printf("Recv error, %d\n", WSAGetLastError());
	//	//remove from sockets
	//	return;
	//}
	//else if (result == 0)
	//{
	//	printf("client has disconnected.\n");
	//}

	//BytesRecv += result;

	//if (BytesRecv > 4)
	//{
	//	BufData.length = //convert length here
	//}

	//if (BufData.bytesReveiced == BufData.length)
	//{
	//	ParseMessage(BufData, toString());
	//}
}

void addSocketInformation(SOCKET s)
{
	// Prepare SocketInfo structure for use
	SocketInfo sInfo = {
		g_IDCounter,
		Buffer(),
		s,
		0,
		0,
	};
	//set the id and increment it 
	g_IDCounter++;
	g_theSockets.push_back(sInfo);
}

void freeSocketInformation(int Index)
{
	SocketInfo sInfo = g_theSockets[Index];
	closesocket(sInfo.socket);
	// Squash the socket array
	g_theSockets.erase(g_theSockets.begin(),g_theSockets.begin()+Index);
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
