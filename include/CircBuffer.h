/*
 * CircBuffer.h
 *
 *  Created on: 19.01.2020
 *      Author: RedOne87
 */

#include <stdint.h>
#include <stdbool.h>

#ifndef INCLUDE_CIRCBUFFER_H_
#define INCLUDE_CIRCBUFFER_H_

#define CIRC_BUFFER_SIZE 512

typedef struct
{
	uint8_t data[CIRC_BUFFER_SIZE];
	uint32_t read_pos;
	uint32_t write_pos;
//	uint32_t count;

} circ_buffer_t;

typedef enum
{
	CIRC_BUFFER_ERR_OK = 0,
	CIRC_BUFFER_ERR_EMPTY,
	CIRC_BUFFER_ERR_FULL,
	CIRC_BUFFER_ERR_NULL,
	CIRC_BUFFER_ERR_OFFSET,

} circ_buffer_error_t;

void circ_buffer_init(circ_buffer_t * buffer);
circ_buffer_error_t circ_buffer_push(circ_buffer_t * buffer, uint8_t data);
circ_buffer_error_t circ_buffer_pop(circ_buffer_t * buffer, uint8_t *out_data);
circ_buffer_error_t circ_buffer_peek(circ_buffer_t * buffer, uint32_t offset, uint8_t *out_data);
circ_buffer_error_t circ_buffer_filled(circ_buffer_t * buffer, uint32_t *out_number);
circ_buffer_error_t circ_buffer_available(circ_buffer_t * buffer, uint32_t *out_number);
circ_buffer_error_t circ_buffer_discard(circ_buffer_t * buffer, uint32_t number_bytes);

#endif /* INCLUDE_CIRCBUFFER_H_ */
