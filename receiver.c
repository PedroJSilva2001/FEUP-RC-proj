/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "msg_macros.h"
#include "./data_link/packet.h"
#include "./data_link/dl.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define LLOPEN_ERR 1
#define LLCLOSE_ERR 3

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
  if (port_fd = llopen(RECEIVER, argv[1]),
      port_fd == -1) {
    perror("Serial port connection not successful");
    return LLOPEN_ERR;
  }
  /*
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
  }*/

  // If received message is correct
/*
  if (check_msg(SET_packet, FRAME_ADDR_EM, FRAME_CTRL_SET, &state) == 0) {
   
    printf("%ld bytes read from the serial port\n", sizeof SET_packet / sizeof SET_packet[0]);
    printf("Message received: %x %x %x %x %x\n", SET_packet[0], SET_packet[1], SET_packet[2], SET_packet[3], SET_packet[4]);

    for (i = 0; i < 5 ; i++) {
      if (n = write(port_fd, &UA_packet[i], 1) < 0) {
        perror(argv[1]);
        exit(-3);
      }               
    }
    printf("%ld bytes written to the serial port. \n", sizeof UA_packet / sizeof UA_packet[0]);
  }*/

  if (llclose(port_fd, RECEIVER) < 0) {
    return LLCLOSE_ERR;
  }
}
