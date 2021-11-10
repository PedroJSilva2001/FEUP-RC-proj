/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdint.h>
#include "msg_macros.h"
#include "./data_link/packet.h"
#include "./data_link/dl.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */

state_t state = START;  /* State Machine */

int check_set_msg(uint8_t *SET) {
    
    while (state != STOP) {
    
        switch (state) {
            case START:
                if (SET[0] == FRAME_FLAG)
                    state = FLAG; 
                
            case FLAG:
                if (SET[1] == FRAME_ADDR_EM)
                    state = A;
                else if (SET[1] == FRAME_FLAG)
                  state = FLAG;
                else {
                    state = START; 
                    return 1;
                }
                    
            case A:
                if (SET[2] == FRAME_CTRL_SET)
                    state = BCC;
                else if (SET[2] == FRAME_FLAG)
                  state = FLAG;
                else {
                    state = START; 
                    return 1;
                }
                    
            case C:
                if (SET[3] == SET[1] ^ SET[2])
                    state = BCC;
                else if (SET[3] == FRAME_FLAG)
                  state = FLAG;
                else {
                    state = START; 
                    return 1;
                }
                    
            case BCC:
                if (SET[4] == FRAME_FLAG)
                    state = STOP;
                else {
                    state = START; 
                    return 1;
                } 
        }
    }
    return 0;
}

int main(int argc, char** argv) {
  if ( (argc < 2) || 
        ((strcmp("/dev/ttyS0", argv[1])!=0) && 
        (strcmp("/dev/ttyS1", argv[1])!=0)) && 
        (strcmp("/dev/ttyS10", argv[1])!=0) &&
        (strcmp("/dev/ttyS11", argv[1])!=0)) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  int port_fd;
  termios_t oldtio;
  if (port_fd = llopen(RECEIVER, argv[1]) ,
      port_fd == -1) {
    perror("Serial port connection");
    return 1;
  }
  
  uint8_t SET_packet[CTRL_PACKET_SIZE];
  uint8_t UA_packet[CTRL_PACKET_SIZE];

  create_control_packet(FRAME_CTRL_UA, FRAME_ADDR_REC, UA_packet);


  int n;
  int i = 0;
  
  for (; i < 5 ; i++) {
    if (n = read(port_fd, &SET_packet[i], 1) , 
        n < 0) {
      perror(argv[1]);
      exit(-4);
    }               
  }
  // If received Message is correct
  if (check_set_msg(&SET_packet) == 0) {
   
    printf("%d bytes read from the serial port\n", sizeof SET_packet / sizeof SET_packet[0]);
    printf("Message received: %x %x %x %x %x\n", SET_packet[0], SET_packet[1], SET_packet[2], SET_packet[3], SET_packet[4]);

    for (i = 0; i < 5 ; i++) {
      if (n = write(port_fd, &UA_packet[i], 1) < 0) {
        perror(argv[1]);
        exit(-3);
      }               
    }
    printf("%d bytes written to the serial port. \n", sizeof UA_packet / sizeof UA_packet[0]);
  }
  
  return llclose(port_fd, RECEIVER);
}
