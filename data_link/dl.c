#include "dl.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>

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
    case EMITTER:;
    case RECEIVER:;
  }

  return port_fd;
}

int llclose(int port_fd, user_type_t type) {
  sleep(2);

  if (tcsetattr(port_fd, TCSANOW, &oldtio) < 0) {
    perror("tcsetattr");
    return 1;
  }
  close(port_fd);
  return 0;
}
/*
static int check_receptor_connection(int port_fd) {

}

static int check_emitter_connection(int port_fd) {

}*/