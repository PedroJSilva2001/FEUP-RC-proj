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
#include "./data_link/frame.h"
#include "./data_link/dl.h"
#include "./app_layer/app_layer.h"

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define LLOPEN_ERR 1
#define LLCLOSE_ERR 3

int main(int argc, char** argv) {
  if ((argc < 3) || 
      ((strcmp("/dev/ttyS0", argv[1])!=0) && 
      (strcmp("/dev/ttyS1", argv[1])!=0)) && 
      (strcmp("/dev/ttyS10", argv[1])!=0) &&
      (strcmp("/dev/ttyS11", argv[1])!=0)) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1 filename.png\n");
    exit(1);
  }

  int port_fd;
  const char *port = &argv[1][strlen("/dev/ttyS")];
  if (port_fd = llopen(atoi(port), RECEIVER),
      port_fd == -1) {
    perror("Serial port connection not successful");
    return LLOPEN_ERR;
  }
  
  if (receive_file(port_fd) < 0) {
    printf("Error in receiving file!\n");
  }

  if (llclose(port_fd, RECEIVER) < 0) {
    return LLCLOSE_ERR;
  }
  return 0;
}