#include "dl.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include "state.h"
#include "frame.h"

#define BAUDRATE B38400
#define MAX_NR_TRIES 3

static void timeout_handler(int sig);
static void init_timeout_handler();

static termios oldtio;
static int tries = 0;
static bool timeout = false;
static char seq_num = 0;

int llopen(int com, user_type type) {
  int port_fd;
  char serial_port[255];
  sprintf(serial_port, "/dev/ttyS%d", com);
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

  char msg_byte = 0;
  char frame[CTRL_FRAME_SIZE];
  ctrl_state state;

  switch (type) {
    case EMITTER: {
        tries = 0;
        create_control_frame(FRAME_CTRL_SET, FRAME_ADDR_EM, frame);
        init_timeout_handler();

        do {
          printf("try nº: %d\n", tries);
          // WRITE SET
          write(port_fd, frame, CTRL_FRAME_SIZE);

          alarm(3);
          
          timeout = false;
          // READ UA
          while (state != C_STOP && !timeout) {
            read(port_fd, &msg_byte, 1);
            printf("e: read %x\n", msg_byte);
            handle_unnumbered_frame_state(msg_byte, FRAME_CTRL_UA, FRAME_ADDR_REC, &state);
          }
        } while (tries <= MAX_NR_TRIES && timeout);
        
        alarm(0);

        if (state == C_STOP) {
          return 0;
        }

        return -1;
    } break;

    case RECEIVER: {
      // READ SET
      while (state != C_STOP) {
        read(port_fd, &msg_byte, 1);
        printf("r: read %x\n", msg_byte);
        handle_unnumbered_frame_state(msg_byte, FRAME_CTRL_SET, FRAME_ADDR_EM, &state);
      }

      // WRITE UA
      if (state == C_STOP) {
        create_control_frame(FRAME_CTRL_UA+1, FRAME_ADDR_REC, frame); //FRAME_CTRL_UA
        write(port_fd, frame, CTRL_FRAME_SIZE);

        printf("r(SENT) ua: write %x %x %x %x %x\n", frame[0], frame[1], frame[2], frame[3] , frame[4]);
        
        // TODO: 
        // O RECEIVER ESTÁ NO ESTADO DISC, RECEBE SET E ENVIA UA E VAI PARA O ESTADO RECEIVE_DATA
        // DEPOIS: PODE RECEBER 2 COISAS: O SET OU TRAMA INFORMAÇÃO
        // SE RECEBER O SET, ENVIA DE NOVO O UA
      }
    } break;
  }

  return port_fd;
}


int llwrite(int fd, char *buffer, int length) {
  char byte;
  tries = 0;
  ctrl_state state = C_START;
  ctrl_state rec_state = C_START;

  information_frame info_frame = create_information_frame(seq_num, buffer, length);

  do {
    printf("try nº: %d\n", tries);
    write(fd, info_frame.bytes, info_frame.size); 

    alarm(3);

    timeout = false;

    while (state != C_STOP && !timeout) {
      read(fd, &byte, 1);
      handle_supervision_frame_state(byte, seq_num, &state);

      if (state == C_REJ_RCV || state == C_RR_RCV) {
        rec_state = state;
      }
    }

  } while (tries <= MAX_NR_TRIES && timeout);
  
  alarm(0);

  if (state == C_STOP) {
    if (rec_state == C_RR_RCV) {
      seq_num = 1-seq_num;
    } else {
      printf("Receiver rejected frame. Resending..\n");
      return llwrite(fd, buffer, length);
    }
    return 0;
  }

  printf("Max time-outs occurred: information frame or Receiver's confirmation was likely lost.\n");
  return -1;
}


int llread(int fd, char *buffer) {  
  info_state state = I_START;
  char *data;
  char byte;
  unsigned int size = 0;

  while (state != I_STOP) {
    read(fd, &byte, 1);
    handle_information_frame_state(byte, seq_num, &state, data, &size);
    // TODO: Check I frame
    // TODO: Update state



  }

   // TODO: CONTINUE




}



int llclose(int port_fd, user_type type) {
  char byte = 0;
  char ctrl_frame[CTRL_FRAME_SIZE];
  //failed_to_read = true;
  ctrl_state state = C_START;
  switch (type) {
    case EMITTER: {
        int tries = 0;

        create_control_frame(FRAME_CTRL_DISC, FRAME_ADDR_EM, ctrl_frame);

        //signal(SIGALRM, signal_handler); 

        while (tries <= MAX_NR_TRIES /*&& failed_to_read*/) {
          tries++;
          printf("nr try: %d\n", tries);

          int n = write(port_fd, ctrl_frame, CTRL_FRAME_SIZE);
          printf("Termination of connection\n");

          alarm(3);
          //failed_to_read = true;
          state = C_START;

          while (state != C_STOP /*&& failed_to_read*/) {
            read(port_fd, &byte, 1);
            printf("e: read %x\n", byte);
            handle_unnumbered_frame_state(byte, FRAME_CTRL_DISC, FRAME_ADDR_REC, &state);
          }

          if (state == C_STOP) {
            /*failed_to_read = false;*/
            printf("DISC received on time\n");
          }

        } while (tries <= MAX_NR_TRIES /*&& failed_to_read*/);

        if (state != C_STOP) {
          printf("Failed: timeout");
          return -1;
        }

        create_control_frame(FRAME_CTRL_UA, FRAME_ADDR_EM, ctrl_frame);
        write(port_fd, ctrl_frame, CTRL_FRAME_SIZE);
        printf("Disconnected\n");
    } break;

    case RECEIVER: {
      // READ DISC
      while (state != C_STOP) {
        read(port_fd, &byte, 1);
        printf("r: read %x\n", byte);
        handle_unnumbered_frame_state(byte, FRAME_CTRL_DISC, FRAME_ADDR_EM, &state);
      }

      if (state != C_STOP) {
        printf("Failed: no DISC received\n");
        return -1;
      }

      // WRITE DISC
      create_control_frame(FRAME_CTRL_DISC, FRAME_ADDR_REC, ctrl_frame);
      write(port_fd, ctrl_frame, CTRL_FRAME_SIZE);

      // READ UA
      state = C_START;

      while (state != C_STOP) {
        read(port_fd, &byte, 1);
        printf("r: read %x\n", byte);
        handle_unnumbered_frame_state(byte, FRAME_CTRL_UA, FRAME_ADDR_EM, &state);
      }

      if (state != C_STOP) {
        printf("Failed: no UA received\n");
        return -1;
      }

      printf("Disconnected\n");
      
    } 
    break;
  }

  sleep(2);
  if (tcsetattr(port_fd, TCSANOW, &oldtio) < 0) {
    perror("tcsetattr");
    return -1;
  }

  close(port_fd);

  return 0;
}

static void timeout_handler(int sig) {
  tries++;
  timeout = true;
}

static void init_timeout_handler() {
  struct sigaction action;
  action.sa_handler = timeout_handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SIGALRM, &action, NULL);
}