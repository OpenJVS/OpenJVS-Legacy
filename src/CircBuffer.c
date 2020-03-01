/*
 * CircBuffer.c
 *
 *  Created on: 19.01.2020
 *      Author: RedOne87
 */

#include "CircBuffer.h"
#include <string.h>

void circ_buffer_init(circ_buffer_t * buffer)
{
	buffer->read_pos = 0;
	buffer->write_pos = 0;
	memset(buffer->data, 0, sizeof(buffer->data));
}

circ_buffer_error_t circ_buffer_push(circ_buffer_t * buffer, uint8_t data)
{
	circ_buffer_error_t retval = CIRC_BUFFER_ERR_OK;

	if(NULL == buffer)
	{
		retval = CIRC_BUFFER_ERR_NULL;
	}

	if(CIRC_BUFFER_ERR_OK == retval)
	{
		if(buffer->write_pos == ((buffer->read_pos - 1 + CIRC_BUFFER_SIZE) % CIRC_BUFFER_SIZE))
		{
			retval = CIRC_BUFFER_ERR_FULL;
		}
	}

	if(CIRC_BUFFER_ERR_OK == retval)
	{
		buffer->data[buffer->write_pos] = data;
		buffer->write_pos = (buffer->write_pos + 1) % CIRC_BUFFER_SIZE;
	}

	return retval;
}

circ_buffer_error_t circ_buffer_discard(circ_buffer_t * buffer, uint32_t number_bytes)
{
	circ_buffer_error_t retval = CIRC_BUFFER_ERR_OK;
	uint32_t number_bytes_available = 0;

	if(NULL == buffer)
	{
		retval = CIRC_BUFFER_ERR_NULL;
	}

	if(CIRC_BUFFER_ERR_OK == retval)
	{
		retval = circ_buffer_filled(buffer, &number_bytes_available);
	}

	if(CIRC_BUFFER_ERR_OK == retval)
	{
		if ((buffer->read_pos == buffer->write_pos) || (number_bytes > (number_bytes_available) ))
		{
			retval = CIRC_BUFFER_ERR_OFFSET;
		}
	}

	if(CIRC_BUFFER_ERR_OK == retval)
	{
		buffer->read_pos = (buffer->read_pos + number_bytes) % CIRC_BUFFER_SIZE;
	}
	return retval;
}


circ_buffer_error_t circ_buffer_pop(circ_buffer_t * buffer, uint8_t *out_data)
{
	circ_buffer_error_t retval = CIRC_BUFFER_ERR_OK;

	if(NULL == buffer)
	{
		retval = CIRC_BUFFER_ERR_NULL;
	}

	if(CIRC_BUFFER_ERR_OK == retval)
	{
		if (buffer->read_pos == buffer->write_pos)
		{
			retval = CIRC_BUFFER_ERR_EMPTY;
		}
	}

	if(CIRC_BUFFER_ERR_OK == retval)
	{
		*out_data = buffer->data[buffer->read_pos];
		buffer->read_pos = (buffer->read_pos + 1) % CIRC_BUFFER_SIZE;
	}
	return retval;
}

circ_buffer_error_t circ_buffer_peek(circ_buffer_t * buffer, uint32_t offset, uint8_t *out_data)
{
	circ_buffer_error_t retval = CIRC_BUFFER_ERR_OK;
	uint32_t number_bytes_available = 0;

	if(NULL == buffer)
	{
		retval = CIRC_BUFFER_ERR_NULL;
	}

	if(CIRC_BUFFER_ERR_OK == retval)
	{
		retval = circ_buffer_filled(buffer, &number_bytes_available);
	}

	if(CIRC_BUFFER_ERR_OK == retval)
	{
		if ((buffer->read_pos == buffer->write_pos) || (offset >= (number_bytes_available) ))
		{
			retval = CIRC_BUFFER_ERR_OFFSET;
		}
	}

	if(CIRC_BUFFER_ERR_OK == retval)
	{
		*out_data = buffer->data[(buffer->read_pos + offset) % CIRC_BUFFER_SIZE];
	}
	return retval;
}

circ_buffer_error_t circ_buffer_filled(circ_buffer_t * buffer, uint32_t *out_number)
{
	circ_buffer_error_t retval = CIRC_BUFFER_ERR_OK;

	if((NULL == buffer) || (NULL == out_number))
	{
		retval = CIRC_BUFFER_ERR_NULL;
	}

	if(CIRC_BUFFER_ERR_OK == retval)
	{
		*out_number = ((buffer->write_pos + CIRC_BUFFER_SIZE - buffer->read_pos) % CIRC_BUFFER_SIZE);
	}

	return retval;
}

circ_buffer_error_t circ_buffer_available(circ_buffer_t * buffer, uint32_t *out_number)
{
	circ_buffer_error_t retval = CIRC_BUFFER_ERR_OK;

	if((NULL == buffer) || (NULL == out_number))
	{
		retval = CIRC_BUFFER_ERR_NULL;
	}

	if(CIRC_BUFFER_ERR_OK == retval)
	{
		*out_number = ((buffer->read_pos + CIRC_BUFFER_SIZE - buffer->write_pos - 1) % CIRC_BUFFER_SIZE);
	}

	return retval;
}
