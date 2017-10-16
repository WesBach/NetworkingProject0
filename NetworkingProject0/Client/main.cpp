
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

class Header {
public:
	//[packet_length][message_id]
	int32_t packet_length;			//in bytes
	int32_t message_id;				//What user is trying to do
};

//global buffer 
Buffer* g_theBuffer;
Header* g_theHeader;

//Protocols method headers
void sendMessage(Header* theHeader,std::string message);
std::string receiveMessage(Buffer& theBuffer);
//TO DO: Client side connection
int main(int argc, char** argv) {
	g_theBuffer = new Buffer(4096);
	g_theHeader = new Header();
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
		//return 1;
	}
	printf("Winsock Initialized\n");


	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}


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
		printf("Unable to connect to server\n");
		WSACleanup();
		return 1;
	}
	printf("Connected to Server\n");


	std::string userInput;
	do {

		userInput = "";
		std::cout << "> ";
		std::cin >> userInput;

		if (userInput.size() > 0)
		{
			int sendResult = send(ConnectSocket, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				char buf[4096] ;
				int bytesReceived = recv(ConnectSocket, g_theBuffer->getBufferAsCharArray(), g_theBuffer->GetBufferLength(), 0);
				if (bytesReceived > 0)
				{
					//do the conversion
					std::string receivedPhrase = receiveMessage(*g_theBuffer);
					std::cout << "Phrase: " << receivedPhrase << std::endl;
				}
			}
		}

	} while (userInput.size() > 0);
}

void sendMessage(Header* theHeader, std::string message)
{
	theHeader->message_id = 1;
	theHeader->packet_length = message.length();
	g_theBuffer->WriteInt32BE(theHeader->message_id);
	g_theBuffer->WriteInt32BE(theHeader->packet_length);
	g_theBuffer->WriteStringBE(message);
}

std::string receiveMessage(Buffer& theBuffer) {
	Header tempHeader;
	//tempHeader.message_id = theBuffer.ReadInt32BE();
	//tempHeader.packet_length = theBuffer.ReadInt32BE();
	std::string message = theBuffer.ReadStringBE();
	return message;
}
