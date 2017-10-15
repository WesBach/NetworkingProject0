#define UNICODE
#define WIN_32_CHAT_APP_SERVER

//#include <Windows.h>   freaks out if i include this
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Buffer.h"
#include <iostream>
#include <sstream>

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT "5000"	//was 8899
#define DEFAULT_BUFFER_LENGTH 1024
//socket info structure to store all the individual socket information

enum message_ID { JOINROOM, LEAVEROOM , SENDMESSAGE, RECEIVEMESSAGE };

class SocketInfo {
public:
	int ID;
	Buffer buffer[DEFAULT_BUFFER_LENGTH];
	SOCKET socket;
	DWORD bytesSEND;
	DWORD bytesRECV;
};
SocketInfo* g_curSocketInfo = new SocketInfo();

//Header struct for packet length and message id
class Header {
public:
	//[packet_length][message_id]
	int packet_length;			//in bytes
	int message_id;				//What user is trying to do
};

//read packet function
//void readPacket(SOCKET socket);
//get socket function for populating the socket info and pushing back onto
void addSocketInformation(SOCKET socket);
void freeSocketInformation(int Index);

//Protocols method headers
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
void joinRoom(std::string &roomName);
void leaveRoom(std::string &roomName);

//sockets
std::vector<SocketInfo> g_theSockets;
int g_IDCounter = 0;

int main()
{
	SOCKET ListenSocket;
	SOCKET AcceptSocket;
	fd_set master;
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
	//socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
	
	ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	printf("Created Listen Socket\n");

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
	printf("Bind Listen Socket\n");

	// Listen()
	if (listen(ListenSocket, 5)) {
		printf("listen() failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	printf("Listen for incoming requests\n");

	ULONG nonBlock = 1;
	//zero out master
	FD_ZERO(&master);

	//for debugging
	char tempBreak;
	bool running = true;

	while (running)
	{
		// Make a copy of the master file descriptor set, this is SUPER important because
		// the call to select() is _DESTRUCTIVE_. The copy only contains the sockets that
		// are accepting inbound connection requests OR messages. 

		// E.g. You have a server and it's master file descriptor set contains 5 items;
		// the listening socket and four clients. When you pass this set into select(), 
		// only the sockets that are interacting with the server are returned. Let's say
		// only one client is sending a message at that time. The contents of 'copy' will
		// be one socket. You will have LOST all the other sockets.

		// SO MAKE A COPY OF THE MASTER LIST TO PASS INTO select() !!!

		fd_set copy = master;

		// See who's talking to us
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		// Loop through all the current connections / potential connect
		for (int i = 0; i < socketCount; i++)
		{
			// Makes things easy for us doing this assignment
			SOCKET sock = copy.fd_array[i];

			// Is it an inbound communication?
			if (sock == ListenSocket)
			{
				// Accept a new connection
				SOCKET client = accept(ListenSocket, nullptr, nullptr);

				// Add the new connection to the list of connected clients
				FD_SET(client, &master);

				// Send a welcome message to the connected client
				//string welcomeMsg = "Welcome to the Awesome Chat Server!\r\n";
				//send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
			}
			else // It's an inbound message
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				// Receive message
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					// Drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					// Check to see if it's a command. \quit kills the server
					if (buf[0] == '\\')
					{
						// Is the command quit? 
						std::string cmd = std::string(buf, bytesIn);
						if (cmd == "\\quit")
						{
							running = false;
							break;
						}

						// Unknown command
						continue;
					}

					// Send message to other clients, and definiately NOT the listening socket

					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != ListenSocket && outSock != sock)
						{
							
							/*ss << "SOCKET #" << sock << ": " << buf << "\r\n";
							std::string strOut = ss.str();*/

							send(outSock, "hi", 2 + 1, 0);
						}
					}
				}
			}
		}
	}

	//while (true) {

	//	//zero the sets
	//	FD_ZERO(&readSet);
	//	FD_ZERO(&writeSet);

	//	//listen for connections on listen socket
	//	FD_SET(ListenSocket, &readSet);

	//	// Set Read and Write notification for each socket based on the
	//	// current state of the buffer.  If there is data remaining in the
	//	// buffer then set the Write set otherwise the Read set
	//	for (int i = 0; i < g_theSockets.size(); i++)
	//	{
	//		if (g_theSockets[i].bytesRECV > g_theSockets[i].bytesSEND)
	//			FD_SET(g_theSockets[i].socket, &writeSet);
	//		else {
	//			if(FD_ISSET(g_theSockets[i].socket, &readSet))
	//				FD_SET(g_theSockets[i].socket, &readSet);
	//		}
	//			
	//	}

	//	if ((totalSocketsInSet = select(0, &readSet, &writeSet, NULL, NULL)) == SOCKET_ERROR)
	//	{
	//		printf("select() returned with error %d\n", WSAGetLastError());
	//		std::cin >> tempBreak;
	//		return 1;
	//	}

	//	// Check for arriving connections on the listening socket.
	//	if (FD_ISSET(ListenSocket, &readSet))
	//	{
	//		if ((AcceptSocket = accept(ListenSocket, NULL, NULL)) != INVALID_SOCKET)
	//		{
	//			// Set the accepted socket to non-blocking mode
	//			nonBlock = 1;
	//			if (ioctlsocket(AcceptSocket, FIONBIO, &nonBlock) == SOCKET_ERROR)
	//			{
	//				printf("ioctlsocket(FIONBIO) failed with error %d\n", WSAGetLastError());
	//				std::cin >> tempBreak;
	//				return 1;
	//			}
	//			else
	//				printf("Accepted client connection.");
	//			//add the socket info to the socket vector
	//			addSocketInformation(AcceptSocket);
	//			//totalSocketsInSet++;
	//		}
	//		else
	//		{
	//			if (WSAGetLastError() != WSAEWOULDBLOCK)
	//			{
	//				printf("accept() failed with error %d\n", WSAGetLastError());
	//				return 1;
	//			}
	//		}
	//	}

	//	// Check each socket for Read and Write notification until the number
	//	// of sockets in Total is satisfied
	//	for (int i = 0; i < g_theSockets.size(); i++)
	//	{
	//		SocketInfo socketInfo = g_theSockets[i];

	//		// If the ReadSet is marked for this socket then this means data
	//		// is available to be read on the socket
	//		if (FD_ISSET(socketInfo.socket, &readSet))
	//		{

	//			flags = 0;
	//			if (recv(socketInfo.socket, (char*)socketInfo.buffer, socketInfo.buffer->GetBufferLength(),flags) == SOCKET_ERROR)
	//			{
	//				if (WSAGetLastError() != WSAEWOULDBLOCK)
	//				{
	//					printf("WSARecv() failed with error %d\n", WSAGetLastError());
	//					std::cin >> tempBreak;
	//					freeSocketInformation(i);
	//				}
	//				else
	//					printf("Reveived information from client");

	//				continue;
	//			}
	//			//====================
	//			//		TO DO
	//			//====================
	//			//do something with the received data 
	//			int breakp = 0;

	//		}//if (FD_ISSET(socketInfo.socket, &readSet))


	//		// If the WriteSet is marked on this socket then this means the internal
	//		// data buffers are available for more data
	//		if (FD_ISSET(socketInfo.socket, &writeSet))
	//		{
	//			if (send(socketInfo.socket, (char*)socketInfo.buffer, socketInfo.buffer->GetBufferLength() , flags) == SOCKET_ERROR)
	//			{
	//				if (WSAGetLastError() != WSAEWOULDBLOCK)
	//				{
	//					printf("WSASend() failed with error %d\n", WSAGetLastError());
	//					std::cin >> tempBreak;
	//					freeSocketInformation(i);
	//				}
	//				continue;
	//			}
	//		}//if (FD_ISSET(socketInfo.socket, &writeSet))
	//	}//for (int i = 0; totalSocketsInSet > 0 && i < g_theSockets.size(); i++)	
	//}//while (true) {

	//clean up
	closesocket(ListenSocket);
	WSACleanup();

	char tempstr;
	std::cin >> tempstr;
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
	//temp filler
	std::cout << "hello" << std::endl;
}

void receiveMessage(Header & theHeader, int & senderNameLength, std::string & senderName, int & messageLength, std::string & message, int & roomNameLength, std::string & roomName)
{
	//temp filler
	std::cout << "thanks" << std::endl;
}

void joinRoom(std::string &roomName)
{
	Header* header = new Header();

	header->message_id = JOINROOM;
	g_curSocketInfo->buffer->WriteInt32BE(roomName.length());
	g_curSocketInfo->buffer->WriteStringBE(roomName);
}

void leaveRoom(std::string &roomName)
{
	Header* header = new Header();

	header->message_id = LEAVEROOM;
	g_curSocketInfo->buffer->WriteInt32BE(roomName.length());
	g_curSocketInfo->buffer->WriteStringBE(roomName);
}


