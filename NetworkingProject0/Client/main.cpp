#include "Buffer.h"
#include <string>
#include <iostream>
#include <conio.h>

#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "5000"
#define DEFAULT_BUFFER_LENGTH 512

//Header class for the message
class Header {
public:
	//[packet_length][message_id]
	int32_t packet_length;			//in bytes
	int32_t message_id;				//What user is trying to do
};

//Global buffer 
Buffer* g_theBuffer;
Header* g_theHeader;

//Protocols method headers
std::string receiveMessage(Buffer& theBuffer);
void readInput(std::vector<std::string>& theStrings, std::string input);
void processCommands(std::vector<std::string>& theCommands);
std::vector<std::string> theCommands;

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

	//Get the address info 
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	//set up the socket
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket() failed with error: %d\n", iResult);
			WSACleanup();
			return 1;
		}
		//connect to the socket
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
	//display commands before loop
	std::cout << "=======================================" << std::endl;
	std::cout << "               Commands:               " << std::endl;
	std::cout << "=======================================" << std::endl;
	std::cout << "Leave Room	: LR (a-z)" << std::endl;
	std::cout << "Join Room		: JR (a-z)" << std::endl;
	std::cout << "Send Message  : SM (followed by message)" << std::endl;
	//string for user input
	std::string userInput;
	do {

		userInput = "";
		std::cout << "> ";
		std::cin >> userInput;

		if (userInput.size() > 0)
		{
			//read the user input
			readInput(theCommands, userInput);
			//process the commands from input 
			processCommands(theCommands);
			//send command
			int sendResult = send(ConnectSocket, g_theBuffer->getBufferAsCharArray(), g_theBuffer->GetBufferLength() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				int bytesReceived = recv(ConnectSocket, g_theBuffer->getBufferAsCharArray(), g_theBuffer->GetBufferLength() + 1, 0);
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


//Name:			receiveMessage
//Purpose:		takes in the message from the server and processes it
std::string receiveMessage(Buffer& theBuffer) {
	Header tempHeader;
	tempHeader.packet_length = theBuffer.ReadInt32BE();
	std::string message = theBuffer.ReadStringBE(tempHeader.packet_length);
	return message;
}

//Name:			processCommands
//Purpose:		processes the user commands and populates the buffer according to message type
void processCommands(std::vector<std::string>& theCommands) {
	if (theCommands.size() > 0)
	{
		//if the command is to leave room
		if (theCommands[0] == "LR" || theCommands[0] == "lr")
		{
			g_theHeader = new Header();
			g_theHeader->message_id = 3;
			g_theBuffer->WriteInt32BE(g_theHeader->message_id);
			g_theHeader->packet_length = theCommands[0].size();
			g_theBuffer->WriteInt32BE(g_theHeader->packet_length);
			g_theBuffer->WriteStringBE(theCommands[0]);
			g_theHeader->packet_length = theCommands[1].size();
			g_theBuffer->WriteStringBE(theCommands[1]);
		}
		

		//if the command is to join room
		if (theCommands[0] == "JR" || theCommands[0] == "jr")
		{
			g_theHeader = new Header();
			g_theHeader->message_id = 2;
			g_theBuffer->WriteInt32BE(g_theHeader->message_id);
			g_theHeader->packet_length = theCommands[0].size();
			g_theBuffer->WriteInt32BE(g_theHeader->packet_length);
			g_theBuffer->WriteStringBE(theCommands[0]);
			g_theHeader->packet_length = theCommands[1].size();
			g_theBuffer->WriteStringBE(theCommands[1]);
		}

		//if the command is to send message
		if (theCommands[0] == "SM" || theCommands[0] == "sm")
		{
			g_theHeader = new Header();
			g_theHeader->message_id = 1;
			g_theBuffer->WriteInt32BE(g_theHeader->message_id);
			g_theHeader->packet_length = theCommands[0].size();
			g_theBuffer->WriteInt32BE(g_theHeader->packet_length);
			g_theBuffer->WriteStringBE(theCommands[0]);
			g_theHeader->packet_length = theCommands[1].size();
			g_theBuffer->WriteStringBE(theCommands[1]);
		}
		
	}
}


//Name:			readInput
//Purpose:		parses the input from the user and separates the first and second strings
void readInput(std::vector<std::string>& theStrings, std::string input) {
	std::string tempString = "";
	for (int i = 0; i < input.size(); i++)
	{
		//if theres a space at the start
		if (input[i] == ' '&& i == 0)
		{
			continue;
		}

		//if theres a space after the first two letters and the temp string has letter already
		if (input[i] == ' '&& i == 3 && tempString.size() >0)
		{
			theStrings.push_back(tempString);
			continue;
		}

		//otherwise just add all characters and spaces
		tempString += input[i];
		if (tempString != "")
			theStrings.push_back(tempString);
	}
}
