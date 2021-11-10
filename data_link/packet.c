#include "packet.h"
#include "../msg_macros.h"

void create_control_packet(uint8_t control, uint8_t address, uint8_t *ctrl_packet) {
  ctrl_packet[0] = FRAME_FLAG;
  ctrl_packet[1] = address;
  ctrl_packet[2] = control;
  ctrl_packet[3] = FRAME_BCC1(address, control);
  ctrl_packet[4] = FRAME_FLAG;
}

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
}