#include "dl.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>

#include "state.h"
#include "../msg_macros.h"

#define BAUDRATE B38400

static termios_t oldtio;

static int check_receptor_connection(int port_fd);
static int check_emitter_connection(int port_fd);

int llopen(user_type_t type, char *serial_port) {
  int port_fd;
  
  /* Open serial port device for reading and writing and not as controlling tty
     because we don't want to get killed if linenoise sends CTRL-C. */
  if (port_fd = open(serial_port, O_RDWR | O_NOCTTY), 
      port_fd < 0) {
    perror(serial_port); 
    return -1;
  }

  if (tcgetattr(port_fd, &oldtio) == -1) {
    perror("tcgetattr");
    return -1;
  }

  struct termios newtio;
  bzero(&newtio, sizeof(newtio));

  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;
  newtio.c_lflag = 0;       // Set input mode (non-canonical, no echo,...)
  newtio.c_cc[VTIME] = 0;   // Inter-character timer unused
  newtio.c_cc[VMIN] = 5;    // Blocking read until 1 char received 

  tcflush(port_fd, TCIOFLUSH);

  if (tcsetattr(port_fd, TCSANOW, &newtio) == -1) {
    perror("tcsetattr");
    return -1;
  }

  switch (type) {
    msg_state_t state = START;
    uint8_t msg_byte = 0;
    int n;
    
    case EMITTER:

      break;

    case RECEIVER: {
      while (state != STOP) {
        read(port_fd, &msg_byte, 1);
        printf("%x\n", msg_byte);
        printf("%d\n", state);
        check_control_packet_byte(msg_byte, FRAME_CTRL_SET, FRAME_ADDR_EM, &state);
      }

      if (state == STOP) {
        uint8_t UA_packet[CTRL_PACKET_SIZE];

        create_control_packet(FRAME_CTRL_UA, FRAME_ADDR_REC, UA_packet);

        printf("Message Received OK\n");
        for (int i = 0; i < 5 ; i++) {
          if (n = write(port_fd, &UA_packet[i], 1) < 0) {
            perror(serial_port);
            exit(-3);
          }               
        }
      }
    }
      break;
  }

  return port_fd;
}

int llclose(int port_fd, user_type_t type) {
  sleep(2);

  if (tcsetattr(port_fd, TCSANOW, &oldtio) < 0) {
    perror("tcsetattr");
    return -1;
  }
  close(port_fd);
  return 0;
}
/*
static int check_receptor_connection(int port_fd) {

}

static int check_emitter_connection(int port_fd) {

}*/