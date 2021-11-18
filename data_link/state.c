#include "state.h"
#include "frame.h"

void check_control_frame_byte(char byte, char control, char address, msg_state *state) {
  switch (*state) {
    case START:
      if (byte == FRAME_FLAG) {
        *state = FLAG_RCV; 
      } break;

    case FLAG_RCV:
      if (byte == address)  {
        *state = A_RCV;
      } else if (byte == FRAME_FLAG) {
        *state = FLAG_RCV;
      } else {
        *state = START; 
      } break;

    case A_RCV:
      if (byte == control) {
        *state = C_RCV;
      } else if (byte == FRAME_FLAG) {
        *state = FLAG_RCV;
      } else {
        *state = START; 
      } break;

    case C_RCV:
      if (byte == address ^ control) {
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