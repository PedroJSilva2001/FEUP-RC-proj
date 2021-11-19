#include "state.h"
#include "frame.h"
#include <stdlib.h>
#include <stdio.h>

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


void check_information_frame_byte(char byte, char control, char address, info_state *state, char *data, unsigned int *size) {
  switch (*state) {
    case ISTART:
      if (byte == FRAME_FLAG) {
        *state = IFLAG_RCV; 
      } break;

    case IFLAG_RCV:
      if (byte == address)  {
        *state = IA_RCV;
      } else if (byte == FRAME_FLAG) {
        *state = IFLAG_RCV;
      } else {
        *state = ISTART; 
      } break;

    case IA_RCV:
      if (byte == control) {
        *state = IC_RCV;
      } else if (byte == FRAME_FLAG) {
        *state = IFLAG_RCV;
      } else {
        *state = ISTART; 
      } break;

    case IC_RCV:
      if (byte == address ^ control) {
        *state = IBCC1_OK;
      } else if (byte == FRAME_FLAG) {
        *state = IFLAG_RCV;
      } else {
        *state = ISTART; 
      } break;

    case IBCC1_OK:
      *state = IDATA;

    case IDATA:
        if (byte == FRAME_FLAG) {
            *state = IBCC2_OK;
        } else {
            data = realloc(data, ++(*size));
            data[(*size)-1] = byte;
            break;
        } 
    
    case IBCC2_OK:
        data = realloc(data, *size);
        char bcc2 = data[(*size)-1];
        data = realloc(data, --(*size));

        int real_size;
        char *destuff_data = destuff_bytes(data, *size, &real_size);

        *size = real_size;
        data = realloc(data, *size);
        data = destuff_data;

        if (bcc2 == frame_BCC2(data, *size)) {
            *state = ISTOP;
        } else {
            *state = ISTART; 
        } break;
     
  }
}