/*
 * buffer.c
 *
 *  Created on: 19.01.2020
 *      Author: RedOne87
 */

#include <stdio.h>
#include <string.h>

#include "buffer.h"

void initBuffer(Buffer *buffer)
{
	buffer->readPosition = 0;
	buffer->writePosition = 0;
	memset(buffer->data, 0, sizeof(buffer->data)); // This is not required, seeing as the buffer reads/writes position?
}

BufferStatus pushToBuffer(Buffer *buffer, uint8_t data)
{
	// Return if the buffer is not set
	if (buffer == NULL)
		return BUFFER_ERROR_NULL;

	// Return if the buffer is full
	if (buffer->writePosition == ((buffer->readPosition - 1 + CIRC_BUFFER_SIZE) % CIRC_BUFFER_SIZE))
		return BUFFER_ERROR_FULL;

	// Write data to the buffer
	buffer->data[buffer->writePosition] = data;
	buffer->writePosition = (buffer->writePosition + 1) % CIRC_BUFFER_SIZE;

	return BUFFER_SUCCESS;
}

BufferStatus discardFromBuffer(Buffer *buffer, uint32_t length)
{
	if (buffer == NULL)
		return BUFFER_ERROR_NULL;

	uint32_t bytesAvailable = 0;
	bufferFilled(buffer, &bytesAvailable);

	if ((buffer->readPosition == buffer->writePosition) || (length > bytesAvailable))
		return BUFFER_ERROR_OFFSET;

	buffer->readPosition = (buffer->readPosition + length) % CIRC_BUFFER_SIZE;

	return BUFFER_SUCCESS;
}

BufferStatus popFromBuffer(Buffer *buffer, uint8_t *outData)
{
	if (buffer == NULL)
		return BUFFER_ERROR_NULL;

	if (buffer->readPosition == buffer->writePosition)
		return BUFFER_ERROR_EMPTY;

	*outData = buffer->data[buffer->readPosition];
	buffer->readPosition = (buffer->readPosition + 1) % CIRC_BUFFER_SIZE;

	return BUFFER_SUCCESS;
}

BufferStatus peekFromBuffer(Buffer *buffer, uint32_t offset, uint8_t *outData)
{
	if (buffer == NULL)
		return BUFFER_ERROR_NULL;

	uint32_t bytesAvailable = 0;
	bufferFilled(buffer, &bytesAvailable);

	if ((buffer->readPosition == buffer->writePosition) || (offset >= (bytesAvailable)))
		return BUFFER_ERROR_OFFSET;

	*outData = buffer->data[(buffer->readPosition + offset) % CIRC_BUFFER_SIZE];

	return BUFFER_SUCCESS;
}

BufferStatus bufferFilled(Buffer *buffer, uint32_t *bytes)
{
	if (buffer == NULL || bytes == NULL)
		return BUFFER_ERROR_NULL;

	*bytes = (buffer->writePosition + CIRC_BUFFER_SIZE - buffer->readPosition) % CIRC_BUFFER_SIZE;

	return BUFFER_SUCCESS;
}

BufferStatus bufferAvailable(Buffer *buffer, uint32_t *bytes)
{
	if (buffer == NULL || bytes == NULL)
		return BUFFER_ERROR_NULL;

	*bytes = (buffer->readPosition + CIRC_BUFFER_SIZE - buffer->writePosition - 1) % CIRC_BUFFER_SIZE;

	return BUFFER_SUCCESS;
}

void printBuffer(Buffer *buffer)
{
	uint8_t data;
	BufferStatus bufferStatus;
	uint32_t bytesAvailable = 0;
	bufferFilled(buffer, &bytesAvailable);
	for (uint32_t i = 0; i < bytesAvailable; i++)
	{
		bufferStatus = peekFromBuffer(buffer, i, &data);
		if (bufferStatus != BUFFER_SUCCESS)
		{
			printf("Error: peekFromBuffer failed with error %d\n", bufferStatus);
			return;
		}
		printf("%02X", data);
	}
	printf("\n");
}
