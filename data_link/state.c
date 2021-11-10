#include "state.h"
#include "../msg_macros.h"

#include <stdio.h>

void check_control_packet_byte(uint8_t byte, uint8_t control, uint8_t address, msg_state_t *state) {
  switch (*state) {
    case START:
      if (byte == FRAME_FLAG) {
        *state = FLAG_RCV; 
      } break;

    case FLAG_RCV:
      if (byte == FRAME_ADDR_EM)  {
        *state = A_RCV;
      } else if (byte == FRAME_FLAG) {
        *state = FLAG_RCV;
      } else {
        *state = START; 
      } break;

    case A_RCV:
      if (byte == FRAME_CTRL_SET) {
        *state = C_RCV;
      } else if (byte == FRAME_FLAG) {
        *state = FLAG_RCV;
      } else {
        *state = START; 
      } break;

    case C_RCV:
      if (byte == control ^ address) {
        *state = BCC_OK;
      } else if (byte == FRAME_FLAG) {
        *state = FLAG_RCV;
      } else {
        *state = START; 
      } break;

    case BCC_OK:
      if (byte == FRAME_FLAG) {
        *state = STOP;
      } else {
        *state = START; 
      } break;
  }
}