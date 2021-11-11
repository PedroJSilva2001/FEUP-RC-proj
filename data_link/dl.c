#include "dl.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <signal.h>

#include "state.h"
#include "../msg_macros.h"
#include "packet.h"

#define BAUDRATE B38400

void atende();

static termios_t oldtio;

int flag=1, tries=1;
int ua_received = 0;
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
    
    case EMITTER: {
        /*uint8_t SET_packet[CTRL_PACKET_SIZE];
        uint8_t UA_packet[CTRL_PACKET_SIZE];

        create_control_packet(FRAME_CTRL_SET, FRAME_ADDR_EM, SET_packet);

        (void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao

        while (tries < 4 && flag) {
          
          if(flag){
            int n = write(port_fd, SET_packet, CTRL_PACKET_SIZE);
            ua_received = read(port_fd, UA_packet, CTRL_PACKET_SIZE);
            alarm(3);                 // activa alarme de 3s
            
          }

        }*/
    }

    break;

    case RECEIVER: {
      while (state != STOP) {
        read(port_fd, &msg_byte, 1);
        printf("read %x\n", msg_byte);
        check_control_packet_byte(msg_byte, FRAME_CTRL_SET, FRAME_ADDR_EM, &state);
      }

      if (state == STOP) {

        uint8_t UA_packet[CTRL_PACKET_SIZE];

        create_control_packet(FRAME_CTRL_UA, FRAME_ADDR_REC, UA_packet);
        
        int n = write(port_fd, UA_packet, CTRL_PACKET_SIZE);

        if (n < 0) {
          perror(serial_port);
          return -1;
        }
/*
        if (n < CTRL_PACKET_SIZE) {
          if (n = write(port_fd, UA_packet, CTRL_PACKET_SIZE - n) , 
              n < 0) {
            perror(serial_port);
            return -1;
          }
        }*/
      }
    } break;
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

void atende()                   // atende alarme
{
	printf("alarme # %d\n", tries);

  if (ua_received == CTRL_PACKET_SIZE) {
    flag = 0;
    tries = 1;
    return;
  }
	flag = 1;  //se for 0 => recebeu UA
	tries++;
}