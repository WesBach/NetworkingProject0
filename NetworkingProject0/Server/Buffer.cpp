#include "Buffer.h"

Buffer::Buffer(size_t size) {
	this->mBuffer = std::vector<char>(size);
}

Buffer::Buffer() {
	return;
}

Buffer::~Buffer() {
	return;
}

//std::vector<uint8_t> Buffer::getBuffer()
//{
//	return mBuffer;
//}

std::vector<char>& Buffer::getBuffer()
{
	return mBuffer;
}

char* Buffer::getBufferAsCharArray() {
	return &mBuffer[0];
}

int Buffer::GetBufferLength() {
	return this->mBuffer.size();
}

//int
//INT32
int Buffer::ReadInt32BE(void) {
	int32_t value = mBuffer[mReadIndex] << 24;
	value |= mBuffer[mReadIndex + 1] << 16;
	value |= mBuffer[mReadIndex + 2] << 8;
	value |= mBuffer[mReadIndex + 3];
	mReadIndex += 4;
	return value;
}

int Buffer::ReadInt32BE(size_t index) {
	//little endian is [index], [index] <<4, [index]<<16, [index]<<24
	int32_t value = mBuffer[index] << 24;
	value |= mBuffer[index + 1] << 16;
	value |= mBuffer[index + 2] << 8;
	value |= mBuffer[index + 3];
	return value;
}

void Buffer::WriteInt32BE(int32_t value) {
	mBuffer.push_back(value >> 24);
	++mWriteIndex;
	mBuffer.push_back(value >> 16); //bit shift by 8 to store 8 bits in each buffer slot
	++mWriteIndex;
	mBuffer.push_back(value >> 8);
	++mWriteIndex;
	mBuffer.push_back(value);
	++mWriteIndex;
}

void Buffer::WriteInt32BE(size_t index, int32_t value) {
	mBuffer[index] = value >> 24;
	++mWriteIndex;
	mBuffer[index + 1] = value >> 16;
	++mWriteIndex;
	mBuffer[index + 2] = value >> 8;
	++mWriteIndex;
	mBuffer[index + 3] = value;
	++mWriteIndex;
}

//UNSIGNED SHORT
void Buffer::WriteUShortBE(size_t index, unsigned short value) {
	mBuffer[index] = value >> 8;
	++mWriteIndex;
	mBuffer[index + 1] = value;
	++mWriteIndex;
}

void Buffer::WriteUShortBE(unsigned short value) {
	mBuffer.push_back(value >> 8);
	++mWriteIndex;
	mBuffer.push_back(value);
	++mWriteIndex;
}

unsigned short Buffer::ReadUShortBE(size_t index) {
	unsigned short value = mBuffer[index] << 8;
	value |= mBuffer[index + 1];
	return value;
}

unsigned short Buffer::ReadUShortBE(void) {
	unsigned short value = mBuffer[mReadIndex] << 8;
	value |= mBuffer[mReadIndex + 1];
	return value;
}


//SHORT
void Buffer::WriteShortBE(size_t index, short value) {
	mBuffer[index + 1] = value >> 8;
	++mWriteIndex;
	mBuffer[index + 1] = value;
	++mWriteIndex;
}

void Buffer::WriteShortBE(short value) {
	mBuffer.push_back(value >> 8);
	++mWriteIndex;
	mBuffer.push_back(value);
	++mWriteIndex;
}

short Buffer::ReadShortBE(size_t index) {
	short value = mBuffer[index] << 8;
	value |= mBuffer[index + 1];
	return value;
}

short Buffer::ReadShortBE(void) {
	short value = mBuffer[mReadIndex] << 8;
	value |= mBuffer[mReadIndex + 1];
	return value;
}

//TO DO: string conversion(not really converting anything)
void Buffer::WriteStringBE(size_t index, std::string value) {

}

void Buffer::WriteStringBE(std::string value) {
	for (int i = 0; i < value.size(); i++)
	{
		mBuffer.push_back(value[i]);
	}
}

std::string Buffer::ReadStringBE(size_t index, int length) {
	std::string phrase = "";
	for (int i = index; i < length; i++)
	{
		phrase += mBuffer[mReadIndex];
	}
	return phrase;
}

std::string Buffer::ReadStringBE(int length) {
	std::string phrase = "";
	for (int i = mReadIndex; i < length; i++)
	{
		phrase += mBuffer[mReadIndex];
	}
	return phrase;
}

