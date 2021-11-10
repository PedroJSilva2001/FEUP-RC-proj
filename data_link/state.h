#ifndef DATA_LINK_STATE_H
#define DATA_LINK_STATE_H

#include <stdint.h>

typedef enum {
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    STOP
} msg_state_t;

int check_control_packet_byte(uint8_t *ctrl_byte, uint8_t control, uint8_t address, msg_state_t *state);


#endif