#include "state.h"
#include "../msg_macros.h"

int check_control_packet_byte(uint8_t *byte, uint8_t control, uint8_t address, msg_state_t *state) {
  while (*state != STOP) {
    switch (*state) {
      case START:
        if (*byte == FRAME_FLAG) {
          state = FLAG_RCV; 
        }

      case FLAG_RCV:
        if (*byte == FRAME_ADDR_EM)  {
          *state = A_RCV;
        } else if (*byte == FRAME_FLAG)
          *state = FLAG_RCV;
        else {
          *state = START; 
        }
              
      case A_RCV:
        if (*byte == FRAME_CTRL_SET) {
          *state = BCC_OK;
        } else if (*byte == FRAME_FLAG) {
          *state = FLAG_RCV;
        } else {
          *state = START; 
        }
              
      case C_RCV:
        if (*byte == control ^ address) {
          *state = BCC_OK;
        }
        else if (*byte == FRAME_FLAG)
          *state = FLAG_RCV;
        else {
          *state = START; 
        }
              
      case BCC_OK:
        if (*byte == FRAME_FLAG)
          *state = STOP;
        else {
          *state = START; 
        } 
    }
  }
  return *state == START? 0 : -1;
}
/*
int check_msg(uint8_t *msg, uint8_t address, uint8_t control, state_t *state) {
    
    while (*state != STOP) {
    
        switch (*state) {
            case START:
                if (msg[0] == FRAME_FLAG)
                    *state = FLAG; 
                
            case FLAG:
                if (msg[1] == address)
                    *state = A;
                else if (msg[1] == FRAME_FLAG)
                    *state = FLAG;
                else {
                    *state = START; 
                    return 1;
                }
                    
            case A:
                if (msg[2] == control)
                    *state = BCC;
                else if (msg[2] == FRAME_FLAG)
                    *state = FLAG;
                else {
                    *state = START; 
                    return 1;
                }
                    
            case C:
                if (msg[3] == msg[1] ^ msg[2])
                    *state = BCC;
                else if (msg[3] == FRAME_FLAG)
                    *state = FLAG;
                else {
                    *state = START; 
                    return 1;
                }
                    
            case BCC:
                if (msg[4] == FRAME_FLAG)
                    *state = STOP;
                else {
                    *state = START; 
                    return 1;
                } 
        }
    }
    return 0;
}*/