#ifndef DATA_LINK_PACKET_H
#define DATA_LINK_PACKET_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../msg_macros.h"

#define INFO_PACKET_MIN_SIZE 6

void create_control_packet(uint8_t control, uint8_t address, uint8_t *ctrl_packet);

void create_information_packet(uint8_t control, uint8_t address, uint8_t *data, int data_length ,uint8_t *info_packet);

void stuffing(uint8_t *info_packet, uint8_t *stuffed_info_packet, unsigned int length);

#endif
