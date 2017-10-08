#include "Buffer.h"


Buffer::Buffer(size_t size) {

}

//int
void Buffer::WriteInt32BE(size_t index, int value) {

}
void Buffer::WriteInt32BE(int value) {
}
int  Buffer::ReadInt32BE(size_t index) {
	return 1;
}
int Buffer::ReadInt32BE(void){
	return 1;
}

//unsigned short
void Buffer::WriteUShortBE(size_t index, unsigned short value) {

}
void Buffer::WriteUShortBE(unsigned short value) {

}
unsigned short Buffer::(size_t index) {
	return 1;
}
unsigned short Buffer::ReadUShortBE(void) {
	return 1;
}

//short
void Buffer::WriteShortBE(size_t index, short value) {

}
void Buffer::WriteShortBE(short value) {

}
short Buffer::ReadShortBE(size_t index) {
	return 1;
}
short Buffer::ReadShortBE(void) {
	return 1;
}

//TO DO: string conversion(not really converting anything)
void Buffer::WriteStringBE(size_t index, std::string value) {

}
void Buffer::WriteStringBE(std::string value) {

}
short Buffer::ReadStringBE(size_t index) {
	return 1;
}
short Buffer::ReadStringBE(void) {
	return 1;
}

