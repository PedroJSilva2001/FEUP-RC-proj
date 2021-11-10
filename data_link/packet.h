#ifndef DATA_LINK_PACKET_H
#define DATA_LINK_PACKET_H

#include <stdint.h>
#include "../msg_macros.h"

void create_control_packet(uint8_t control, uint8_t address, uint8_t *ctrl_packet);

int check_msg(uint8_t *msg, uint8_t address, uint8_t control, state_t *state);

//uint8_t *create_information_packet(uint8_t *inf, size_t len);

#endif