/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "msg_macros.h"
#include "./data_link/frame.h"
#include "./data_link/dl.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define LLOPEN_ERR 1
#define LLCLOSE_ERR 3

int main(int argc, char** argv) {
  /*
  if ((argc < 2) || 
      ((strcmp("/dev/ttyS0", argv[1])!=0) && 
      (strcmp("/dev/ttyS1", argv[1])!=0)) && 
      (strcmp("/dev/ttyS10", argv[1])!=0) &&
      (strcmp("/dev/ttyS11", argv[1])!=0)) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }*/

  int port_fd;
  if (port_fd = llopen(atoi(argv[1]), EMITTER) ,
      port_fd == -1) {
    printf("Serial port connection was not established correctly");
    return 1;
  }
  
  /*
  char UA_packet[CTRL_PACKET_SIZE];
  char SET_packet[CTRL_PACKET_SIZE];

  create_control_packet(FRAME_CTRL_SET, FRAME_ADDR_EM, SET_packet);
  printf("Message Sent: %x %x %x %x %x \n", SET_packet[0], SET_packet[1], SET_packet[2], SET_packet[3], SET_packet[4]);

  int i = 0;
  int n;
  for (; i < 5 ; i++) {
    if (n = write(port_fd, &SET_packet[i], 1) ,
        n < 0) {
      perror(argv[1]);
      exit(-3);
    }               
  }  */        
  /*
  for (i = 0; i < 5 ; i++) {
    if (n = read(port_fd, &UA_packet[i], 1) ,
        n < 0) {
      perror(argv[1]);
      exit(-4);
    }               
  }

  // If received message is correct
  if (check_msg(UA_packet, FRAME_ADDR_REC, FRAME_CTRL_UA, &state) == 0) {
    printf("%ld bytes read from the serial port\n", sizeof UA_packet / sizeof UA_packet[0]);
    printf("Message received: %x %x %x %x %x\n", UA_packet[0], UA_packet[1], UA_packet[2], UA_packet[3], UA_packet[4]);
  }
*/
  if (llclose(port_fd, EMITTER) < 0) {
    return LLCLOSE_ERR;
  }
}
