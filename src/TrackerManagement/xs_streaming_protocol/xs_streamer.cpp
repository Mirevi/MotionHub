/*! \file
	\section FileCopyright Copyright Notice
	This is free and unencumbered software released into the public domain.

	Anyone is free to copy, modify, publish, use, compile, sell, or
	distribute this software, either in source code form or as a compiled
	binary, for any purpose, commercial or non-commercial, and by any
	means.

	In jurisdictions that recognize copyright laws, the author or authors
	of this software dedicate any and all copyright interest in the
	software to the public domain. We make this dedication for the benefit
	of the public at large and to the detriment of our heirs and
	successors. We intend this dedication to be an overt act of
	relinquishment in perpetuity of all present and future rights to this
	software under copyright law.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
	OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
	ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
*/

#include "xs_streamer.h"

/*!
	Intel processors use "Little Endian" byte order:

	Base_Address+0 Byte0
	Base_Address+1 Byte1
	Base_Address+2 Byte2
	Base_Address+3 Byte3

	-------------------------
	"Big Endian" :

	Base_Address+0 Byte3
	Base_Address+1 Byte2
	Base_Address+2 Byte1
	Base_Address+3 Byte0
*/

Streamer::Streamer(const XsByteArray& arr)
	: m_offset(0)
{
	// reference to the real data
	m_array = &arr;

	// check Endianess
	int num = 1;
	if(*(char *)&num == 1)
	{
		m_endianess = Little_Endian;
	}
	else
	{
		m_endianess = Big_Endian;
	}
}

/*! Destructor */
Streamer::~Streamer()
{
}

/*! Extract 4 byte from the ByteArray and Store the value into a int32_t (4 byte) variable */
void Streamer::read(int32_t &destination)
{
	char byte_array[4];

	// copy part of the original byte array into a temp array
	memcpy(byte_array, m_array->data()+m_offset, sizeof(destination));

	if (m_endianess == Little_Endian)
		destination = (byte_array[0] << 24) | ((byte_array[1] & 0xff) << 16) | ((byte_array[2] & 0xff) << 8) | (byte_array[3] & 0xff);
	else
		destination = (byte_array[3] << 24) | ((byte_array[2] & 0xff) << 16) | ((byte_array[1] & 0xff) << 8) | (byte_array[0] & 0xff);

	// increase the index
	m_offset += 4;
}

/*! Extract 1 byte from the ByteArray and Store the value into a int8_t (1 byte) variable */
void Streamer::read(int8_t &destination)
{
	int8_t res;

	memcpy(&res, m_array->data()+m_offset, 1);
	destination = (int)res & 0xFF;

	// increase the index
	m_offset++;
}

/*! Extract 2 byte from the ByteArray and Store the value into a int16_t (2 byte) variable */
void Streamer::read(int16_t &destination)
{
	char byte_array[2];

	// copy part of the original byte array into a temp array
	memcpy(byte_array, m_array->data()+m_offset, 2);

	if (m_endianess == Little_Endian)
		destination = ((byte_array[0] & 0xff) << 8) | (byte_array[1] & 0xff);
	else
		destination = ((byte_array[1] & 0xff) << 8) | (byte_array[0] & 0xff);

	// increase the index
	m_offset += 2;
}

/*! Extract 2 byte from the ByteArray and Store the value into a uint16_t (2 byte) variable */
void Streamer::read(uint16_t &destination)
{
	uint8_t byte_array[2];

	// copy part of the original byte array into a temp array
	memcpy(byte_array, m_array->data()+m_offset, 2);

	if (m_endianess == Little_Endian)
		destination = ((byte_array[0] & 0xff) << 8) | (byte_array[1] & 0xff);
	else
		destination = ((byte_array[1] & 0xff) << 8) | (byte_array[0] & 0xff);

	// increase the index
	m_offset += 2;
}

/*! Extract 1 byte from the ByteArray and Store the value into a uint8_t (1 byte) variable */
void Streamer::read(uint8_t &destination)
{
	uint8_t res;

	memcpy(&res, m_array->data()+m_offset, 1);
	destination = (int)res & 0xFF;

	// increase the index
	m_offset++;
}

/*! Extract 4 byte from the ByteArray and Store the value into a float (4 byte) variable */
void Streamer::read(float &destination)
{
	float output = 0.0;

	unsigned char byte_array[4];
	// copy part of the original byte array into a temp array
	memcpy(&byte_array, m_array->data()+m_offset, sizeof(destination));

	if (m_endianess == Little_Endian)
	{
		*((unsigned char*)(&output) + 3) = byte_array[0];
		*((unsigned char*)(&output) + 2) = byte_array[1];
		*((unsigned char*)(&output) + 1) = byte_array[2];
		*((unsigned char*)(&output) + 0) = byte_array[3];
	}
	else
	{
		*((unsigned char*)(&output) + 3) = byte_array[3];
		*((unsigned char*)(&output) + 2) = byte_array[2];
		*((unsigned char*)(&output) + 1) = byte_array[1];
		*((unsigned char*)(&output) + 0) = byte_array[0];
	}

	destination = output;

	// increase the index
	m_offset += 4;
}

/*! Extract "size" byte from the ByteArray and Store the value into a string variable */
void Streamer::read(std::string& str, int numChars)
{
	char * buffer = new char[numChars + 1];

	memcpy(buffer, m_array->data()+m_offset, numChars);
	buffer[numChars] = '\0';

	str = buffer;
	delete[] buffer;

	// increase the index
	m_offset += numChars;
}
