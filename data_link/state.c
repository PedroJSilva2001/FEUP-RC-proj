#include "state.h"
#include "frame.h"
#include <stdlib.h>
#include <stdio.h>

void handle_unnumbered_frame_state(char byte, char control, char address, ctrl_state *state) {
  switch (*state) {
    case C_START:
      printf("Cstart\n");
      if (byte == FRAME_FLAG) {
        *state = C_FLAG_RCV; 
      } break;

    case C_FLAG_RCV:
      printf("cflagrcv\n");

      if (byte == address)  {
        *state = C_A_RCV;
      } else if (byte == FRAME_FLAG) {
        *state = C_FLAG_RCV;
      } else {
        *state = C_START; 
      } break;

    case C_A_RCV:
      printf("Carcv\n");

      if (byte == control) {
        *state = C_C_RCV;
      } else if (byte == FRAME_FLAG) {
        *state = C_FLAG_RCV;
      } else {
        *state = C_START; 
      } break;

    case C_C_RCV:
      printf("Ccrcv\n");

      if (byte == address ^ control) {
        *state = C_BCC_OK;
      } else if (byte == FRAME_FLAG) {
        *state = C_FLAG_RCV;
      } else {
        *state = C_START; 
      } break;

    case C_BCC_OK:
      printf("cbcc\n");

      if (byte == FRAME_FLAG) {
        *state = C_STOP;
        printf("cstop\n");

      } else {
        *state = C_START; 
      } break;
  }
}


void handle_information_frame_state(char byte, char s, info_state *state, char *data, unsigned int *size) {
  switch (*state) {
    case I_START:
      if (byte == FRAME_FLAG)
        *state = I_FLAG_RCV;
    break;

    case I_FLAG_RCV:
      if (byte == FRAME_ADDR_REC)   // TODO: ERRADO?
        *state = I_INFO_A_RCV;
      else if (byte == FRAME_ADDR_EM)
        *state = I_SET_A_RCV;
      else if (byte == FRAME_FLAG)
        *state = I_FLAG_RCV;
      else
        *state = I_START;
    break;

    case I_INFO_A_RCV:
      if (byte == FRAME_CTRL_INFO(s))
        *state = I_INFO_C_RCV;
      else if (byte == FRAME_FLAG)
        *state = I_FLAG_RCV;
      else
        *state = I_START;
    break;

    case I_SET_A_RCV:
      if (byte == FRAME_CTRL_SET)
        *state = I_SET_C_RCV;
      else if (byte == FRAME_FLAG)
        *state = I_FLAG_RCV;
      else
        *state = I_START;
    break;

    case I_INFO_C_RCV:
      if (byte == FRAME_ADDR_REC ^ FRAME_CTRL_INFO(s))
        *state = I_INFO_BCC1_OK;
      else if (byte == FRAME_FLAG)
        *state = I_FLAG_RCV;
      else
        *state = I_START; 
    break;

    case I_SET_C_RCV:
      if (byte == FRAME_ADDR_EM ^ FRAME_CTRL_SET)
        *state = I_SET_BCC1_OK;
      else if (byte == FRAME_FLAG)
        *state = I_FLAG_RCV;
      else
        *state = I_START; 
    break;

    case I_SET_BCC1_OK:
      if (byte == FRAME_FLAG)
        *state = C_STOP;
      else
        *state = C_START; 
    break;

    case I_INFO_BCC1_OK:
      *state = I_DATA;

    case I_DATA:
      if (byte == FRAME_FLAG) {
        //data = realloc(data, *size);  // this might not work (data is passed as char *)

        int real_size;
        char *destuff_data = destuff_bytes(data, *size, &real_size);
        char bcc2 = destuff_data[(*size)-1];
        *size = real_size - 1;        // Remove BBC2 byte
        data = realloc(data, *size); 
        data = destuff_data;

        if (bcc2 == frame_BCC2(data, *size))
          *state = I_BCC2_OK;
        else
          *state = I_BBC2_NOT_OK;
      }
      else {
        data = realloc(data, ++(*size));
        data[(*size)-1] = byte;
      }
    break;

    case I_BBC2_NOT_OK:
    
    case I_BCC2_OK:
        if (byte == FRAME_FLAG)
          *state = I_STOP;
        else
          *state = I_START;  
    break;
  }
}

void handle_supervision_frame_state(char byte, char r, ctrl_state *state) {
  switch (*state) {
    case C_START:
      if (byte == FRAME_FLAG)
        *state = I_FLAG_RCV;
    break;
    
    case C_FLAG_RCV:
      if (byte == FRAME_ADDR_EM)
        *state = C_A_RCV;
      else if
        (byte == FRAME_FLAG) *state = C_FLAG_RCV;
      else
        *state = C_START;
    break;

    case C_A_RCV:
      if (byte == FRAME_CTRL_REJ(r))
        *state = C_REJ_RCV;
      else if (byte == FRAME_CTRL_RR(r)) 
        *state = C_RR_RCV; 
      else if (byte == FRAME_FLAG)
        *state = C_FLAG_RCV;
      else
        *state = C_START;
    break;

    case C_REJ_RCV:
      if (byte == FRAME_ADDR_EM ^ FRAME_CTRL_REJ(r))
        *state = C_BCC_OK;
      else if (byte == FRAME_FLAG)
        *state = C_FLAG_RCV;
      else
        *state = C_START; 
    break;

    case C_RR_RCV:
      if (byte == FRAME_ADDR_EM ^ FRAME_CTRL_RR(r))
        *state = C_BCC_OK;
      else if (byte == FRAME_FLAG)
        *state = C_FLAG_RCV;
      else
        *state = C_START; 
    break;

    case C_BCC_OK:
      if (byte == FRAME_FLAG)
        *state = C_STOP;
      else
        *state = C_START; 
    break;
  }
}