#include "state.h"
#include "frame.h"
#include <stdlib.h>
#include <stdio.h>

void check_control_frame_byte(char byte, char control, char address, ctrl_state *state) {
  switch (*state) {
    case C_START:
      if (byte == FRAME_FLAG) {
        *state = C_FLAG_RCV; 
      } break;

    case C_FLAG_RCV:
      if (byte == address)  {
        *state = C_A_RCV;
      } else if (byte == FRAME_FLAG) {
        *state = C_FLAG_RCV;
      } else {
        *state = C_START; 
      } break;

    case C_A_RCV:
      if (byte == control) {
        *state = C_C_RCV;
      } else if (byte == FRAME_FLAG) {
        *state = C_FLAG_RCV;
      } else {
        *state = C_START; 
      } break;

    case C_C_RCV:
      if (byte == address ^ control) {
        *state = C_BCC_OK;
      } else if (byte == FRAME_FLAG) {
        *state = C_FLAG_RCV;
      } else {
        *state = C_START; 
      } break;

    case C_BCC_OK:
      if (byte == FRAME_FLAG) {
        *state = C_STOP;
      } else {
        *state = C_START; 
      } break;
  }
}


void check_information_frame_byte(char byte, char control, char address, info_state *state, char *data, unsigned int *size) {
  switch (*state) {
    case I_START:
      if (byte == FRAME_FLAG) {
        *state = I_FLAG_RCV; 
      } break;

    case I_FLAG_RCV:
      if (byte == address)  {
        *state = I_A_RCV;
      } else if (byte == FRAME_FLAG) {
        *state = I_FLAG_RCV;
      } else {
        *state = I_START; 
      } break;

    case I_A_RCV:
      if (byte == control) {
        *state = I_C_RCV;
      } else if (byte == FRAME_FLAG) {
        *state = I_FLAG_RCV;
      } else {
        *state = I_START; 
      } break;

    case I_C_RCV:
      if (byte == address ^ control) {
        *state = I_BCC1_OK;
      } else if (byte == FRAME_FLAG) {
        *state = I_FLAG_RCV;
      } else {
        *state = I_START; 
      } break;

    case I_BCC1_OK:
      *state = I_DATA;

    case I_DATA:
        if (byte == FRAME_FLAG) {
            *state = I_BCC2_OK;
        } else {
            data = realloc(data, ++(*size));
            data[(*size)-1] = byte;
            break;
        } 
    
    case I_BCC2_OK:
        data = realloc(data, *size);
        char bcc2 = data[(*size)-1];
        data = realloc(data, --(*size));

        int real_size;
        char *destuff_data = destuff_bytes(data, *size, &real_size);

        *size = real_size;
        data = realloc(data, *size);
        data = destuff_data;

        if (bcc2 == frame_BCC2(data, *size)) {
            *state = I_STOP;
        } else {
            *state = I_START; 
        } break;
     
  }
}