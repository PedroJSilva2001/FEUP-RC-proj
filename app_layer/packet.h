#ifndef APP_LAYER_packet_H
#define APP_LAYER_packet_H

#include <stdint.h>
#include "../data_link/frame.h"

/* Control Packet */
#define PACKET_CTRL_START 0x02
#define PACKET_CTRL_END   0x03
#define PACKET_T_LENGTH   0x00
#define PACKET_T_NAME     0x01

#define CTRL_PACKET_MIN_SIZE 3


/* Data Packet */
#define PACKET_CTRL_DATA 0x01

#define PACKET_DATA_MIN_SIZE 4
#define PACKET_MAX_DATA_SIZE 1024

typedef container packet;

packet create_control_packet(uint8_t control, uint8_t type, unsigned long file_size);
void add_to_control_packet(uint8_t type, unsigned int size, char *data, packet *control_packet);

void create_data_packet(unsigned int sequence_nr, uint8_t* data, unsigned int size, uint8_t *data_packet, unsigned int *packet_length); 

#endif