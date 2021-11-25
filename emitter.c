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
#include "./data_link/frame.h"
#include "./data_link/dl.h"
#include "./app_layer/app_layer.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define LLOPEN_ERR 1
#define LLCLOSE_ERR 3

int main(int argc, char** argv) {
  
  if ((argc < 3) || 
      ((strcmp("/dev/ttyS0", argv[1])!=0) && 
      (strcmp("/dev/ttyS1", argv[1])!=0)) && 
      (strcmp("/dev/ttyS10", argv[1])!=0) &&
      (strcmp("/dev/ttyS11", argv[1])!=0)) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1 filename.jpg\n");
    exit(1);
  }

  // Discover file size
  FILE *file = fopen(argv[2], "r");
  if (file == NULL) {
    printf("File not found!\n");
    return -1;
  }

  if (fseek(file, 0L, SEEK_END) < 0) {
    printf("Error in fseek!\n");
    return -1;
  }
  unsigned long file_size = ftell(file);
  
  if (file_size < 0) {
    printf("Error in ftell!\n");
    return -1;
  }
  fclose(file);

  int port_fd;
  if (port_fd = llopen(argv[1], EMITTER) ,
      port_fd == -1) {
    printf("Serial port connection was not established correctly");
    return 1;
  }

  // Send file
  if (send_file(port_fd, argv[2], file_size) < 0) {
    printf("Error in sending file!\n");
  }

  if (llclose(port_fd, EMITTER) < 0) {
    return LLCLOSE_ERR;
  }
  return 0;
}