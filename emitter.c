/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include "msg_macros.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

state_t state = START;  /* State Machine */

int check_ua_msg(uint8_t *UA) {
    
    while (state != STOP) {
    
        switch (state) {
            case START:
                if (UA[0] == FRAME_FLAG)
                  state = FLAG; 
                 
            case FLAG:
                if (UA[1] == FRAME_ADDR_REC)
                  state = A;
                else if (UA[1] == FRAME_FLAG)
                  continue;
                else {
                  state = START; 
                  return 1;
                }
                    
            case A:
                if (UA[2] == FRAME_CTRL_UA)
                  state = BCC;
                else if (UA[2] == FRAME_FLAG)
                  state = FLAG;
                else {
                  state = START; 
                  return 1;
                }
                    
            case C:
                if (UA[3] == UA[1] ^ UA[2])
                    state = BCC;
                else if (UA[3] == FRAME_FLAG)
                  state = FLAG;
                else {
                    state = START; 
                    return 1;
                }
                    
            case BCC:
                if (UA[4] == FRAME_FLAG)
                    state = STOP;
                else {
                    state = START; 
                    return 1;
                } 
        }
    }
    return 0;
}

int main(int argc, char** argv)
{
  struct termios oldtio,newtio;
  //char buf[255];

  uint8_t SET[] = {FRAME_FLAG, FRAME_ADDR_EM, FRAME_CTRL_SET, 
    FRAME_BCC1(FRAME_ADDR_EM, FRAME_CTRL_SET), FRAME_FLAG};
  uint8_t UA[5];


  if ( (argc < 2) || 
        ((strcmp("/dev/ttyS0", argv[1])!=0) && 
        (strcmp("/dev/ttyS1", argv[1])!=0)) && 
        (strcmp("/dev/ttyS10", argv[1])!=0) &&
        (strcmp("/dev/ttyS11", argv[1])!=0)) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }


  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
  */

  int port_fd;
  if (port_fd = open(argv[1], O_RDWR | O_NOCTTY), 
      port_fd < 0) { 
    perror(argv[1]); 
    exit(-1); 
  }

  /* save current port settings */
  if (tcgetattr(port_fd, &oldtio) == -1) {
    perror("tcgetattr");
    exit(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;
  newtio.c_lflag = 0;       /* set input mode (non-canonical, no echo,...) */
  newtio.c_cc[VTIME] = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN] = 5;    /* blocking read until 5 chars received */

  /*
  VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
  leitura do(s) próximo(s) caracter(es)
  */

  tcflush(port_fd, TCIOFLUSH);

  if (tcsetattr(port_fd, TCSANOW, &newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");


  /*printf("Enter a message: ");
  if (fgets(buf, 255, stdin) == NULL) {
    perror("Error reading from stdin");
    exit(-2);
  }

  int n;
  if (n = write(port_fd, buf, strlen(buf)+1), 
      n < 0) { 
    perror(argv[1]);
    exit(-3); 
  }
  printf("%d bytes written to the serial port\n", n-1);

  while (1) {
    if (n = read(port_fd, &buf[i], 1) < 0) {
      perror(argv[1]);
      exit(-4);
    }               
    if (buf[i++]=='\0') { break; }
  }

  printf("%d bytes read from the serial port\n", n);
  printf("Message echoed back: %s\n", buf);

  sleep(2);
  */  

  int i = 0;
  int n;
  for (; i < 5 ; i++) {
    if (n = write(port_fd, &SET[i], 1) < 0) {
      perror(argv[1]);
      exit(-3);
    }               
  }
  printf("%d bytes written to the serial port\n", sizeof SET / sizeof SET[0]);


  for (i = 0; i < 5 ; i++) {
    if (n = read(port_fd, &UA[i], 1) < 0) {
      perror(argv[1]);
      exit(-4);
    }               
  }

  // If received Message is correct
  if (check_ua_msg(&UA) == 0) {
    printf("%d bytes read from the serial port\n", sizeof UA / sizeof UA[0]);
    printf("Message echoed back: %x %x %x %x %x\n", UA[0], UA[1], UA[2], UA[3], UA[4]);
  }

  sleep(2);
  
  if (tcsetattr(port_fd, TCSANOW, &oldtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  close(port_fd);
  return 0;
}
