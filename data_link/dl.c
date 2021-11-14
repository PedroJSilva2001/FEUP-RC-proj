#include "dl.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <stdbool.h>

#include "state.h"
#include "../msg_macros.h"
#include "packet.h"

#define BAUDRATE B38400

static termios_t oldtio;
msg_state_t state = START;

int MAX_NR_TRIES = 4;
bool failed_to_read = true;

void signal_handler() {                  

	if (state != STOP) {
    failed_to_read = true;
    printf("UA *NOT* received in time\n");
    return;
  }

  printf("UA received in time\n");
}

int llopen(char *serial_port, user_type_t type) {
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

  uint8_t msg_byte = 0;

  switch (type) {
      
    case EMITTER: {
        int tries = 0;
        uint8_t SET_packet[CTRL_PACKET_SIZE];
        uint8_t UA_packet[CTRL_PACKET_SIZE];

        create_control_packet(FRAME_CTRL_SET, FRAME_ADDR_EM, SET_packet);

        (void) signal(SIGALRM, signal_handler); 

        do {
          tries++;
          printf("nr try: %d\n", tries);
          int n = write(port_fd, SET_packet, CTRL_PACKET_SIZE);

          alarm(3);
          failed_to_read = true;
          state = START;

          while (state != STOP && failed_to_read) {
            read(port_fd, &msg_byte, 1);
            printf("e: read %x\n", msg_byte);
            check_control_packet_byte(msg_byte, FRAME_CTRL_UA, FRAME_ADDR_REC, &state);
          }

          if (state == STOP) {
            failed_to_read = false;
            printf("UA received on time\n");
          }

        } while (tries < MAX_NR_TRIES && failed_to_read);
    }

    break;

    case RECEIVER: {
      while (state != STOP) {
        read(port_fd, &msg_byte, 1);
        printf("r: read %x\n", msg_byte);
        check_control_packet_byte(msg_byte, FRAME_CTRL_SET, FRAME_ADDR_EM, &state);
      }

      if (state == STOP) {

        uint8_t UA_packet[CTRL_PACKET_SIZE];
        create_control_packet(FRAME_CTRL_UA, FRAME_ADDR_REC, UA_packet);
        
        int n = write(port_fd, UA_packet, CTRL_PACKET_SIZE);

        printf("r(SENT) ua: write %x %x %x %x %x\n", UA_packet[0], UA_packet[1], UA_packet[2], UA_packet[3] , UA_packet[4]);

        if (n < 0) {
          perror(serial_port);
          return -1;
        }
        
        
        // TODO: 
        // O RECEIVER ESTÁ NO ESTADO DISC, RECEBE SET E ENVIA UA E VAI PARA O ESTADO RECEIVE_DATA
        // DEPOIS: PODE RECEBER 2 COISAS: O SET OU TRAMA INFORMAÇÃO
        // SE RECEBER O SET, ENVIA DE NOVO O UA
      
              
        
        
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


int llwrite(int fd, char *buffer, int length) {
    uint8_t *info_packet;
    
    create_information_packet(FRAME_CTRL_RR(0), FRAME_ADDR_EM, buffer, length, info_packet);


}


int llread(int fd, char *buffer) {






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
