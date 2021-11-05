/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdint.h>
#include "msg_macros.h"

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
  struct termios oldtio, newtio;
  //char buf[255];
  
  uint8_t SET[5];
  uint8_t UA[] = {FRAME_FLAG, FRAME_ADDR_REC, FRAME_CTRL_UA, 
    FRAME_BCC1(FRAME_ADDR_REC, FRAME_CTRL_UA), FRAME_FLAG};

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
  if (tcgetattr(port_fd,&oldtio) == -1) {
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
    leitura do(s) pr�ximo(s) caracter(es)
  */

  tcflush(port_fd, TCIOFLUSH);

  if (tcsetattr(port_fd, TCSANOW, &newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");

  int n;
  int i = 0;
  /*while (1) {
    if (n = read(port_fd, &buf[i], 1) < 0) {
      perror(argv[1]);
      exit(-4);
    }               
    if (buf[i++]=='\0') { break; }
  }
  printf("%d bytes read from the serial port\n", n);
  printf("Message received: %s", buf);
  if (n = write(port_fd, buf, strlen(buf)+1), 
      n < 0) { 
    perror(argv[1]);
    exit(-3); 
  }
  sleep(2);
  printf("%d bytes written to the serial port\n", n-1);*/
  
  for (; i < 5 ; i++) {
    if (n = read(port_fd, &SET[i], 1) < 0) {
      perror(argv[1]);
      exit(-4);
    }               
  }
  
  // If received Message is correct
  if (check_set_msg(&SET) == 0) {
   
    printf("%d bytes read from the serial port\n", sizeof SET / sizeof SET[0]);
    printf("Message received: %x %x %x %x %x\n", SET[0], SET[1], SET[2], SET[3], SET[4]);

    for (i = 0; i < 5 ; i++) {
      if (n = write(port_fd, &UA[i], 1) < 0) {
        perror(argv[1]);
        exit(-3);
      }               
    }
    printf("%d bytes written to the serial port. \n", sizeof UA / sizeof UA[0]);
    
    sleep(2);
  }
  
  tcsetattr(port_fd, TCSANOW, &oldtio);
  close(port_fd);
  return 0;
}
