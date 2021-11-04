/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */


int main(int argc, char** argv) {
  struct termios oldtio, newtio;
  char buf[255];

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
  newtio.c_lflag = 0; /* set input mode (non-canonical, no echo,...) */
  newtio.c_cc[VTIME] = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN] = 5;   /* blocking read until 5 chars received */

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

  int i = 0;
  int n;
  while (1) {
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
  printf("%d bytes written to the serial port\n", n-1);
  sleep(2);
  
  tcsetattr(port_fd, TCSANOW, &oldtio);
  close(port_fd);
  return 0;
}
