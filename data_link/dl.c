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
static uint8_t seq_num = 0;

int llopen(uint8_t serial_port[], user_type type) {
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
  newtio.c_cc[VTIME] = 0;   // Inter-uint8_tacter timer unused
  newtio.c_cc[VMIN] = 1;    // Blocking read until 1 uint8_t received 

  tcflush(port_fd, TCIOFLUSH);

  if (tcsetattr(port_fd, TCSANOW, &newtio) == -1) {
    perror("tcsetattr");
    return -1;
  }

  uint8_t byte = 0;
  uint8_t frame[CTRL_FRAME_SIZE];
  ctrl_state state = C_START;

  switch (type) {
    case EMITTER: {
        tries = 0;
        create_control_frame(FRAME_CTRL_SET, FRAME_ADDR_EM, frame);
        init_timeout_handler();

        do {
          // WRITE SET
          write(port_fd, frame, CTRL_FRAME_SIZE);

          alarm(3);
          
          timeout = false;
          // READ UA
          while (state != C_STOP && !timeout) {
            read(port_fd, &byte, 1);
            handle_unnumbered_frame_state(byte, FRAME_CTRL_UA, FRAME_ADDR_EM, &state);
          }
        } while (tries <= MAX_NR_TRIES && timeout);
        
        alarm(0);

        if (state != C_STOP) {
          printf("\n");
          return -1;
        }
    }
    break;

    case RECEIVER: {
      // READ SET
      while (state != C_STOP) {
        read(port_fd, &byte, 1);
        handle_unnumbered_frame_state(byte, FRAME_CTRL_SET, FRAME_ADDR_EM, &state);
      }

      // WRITE UA
      if (state == C_STOP) {
        create_control_frame(FRAME_CTRL_UA, FRAME_ADDR_EM, frame);
        write(port_fd, frame, CTRL_FRAME_SIZE);
        
      } else {
        printf("\n");
        return -1;
      }
    } 
    break;
  }

  return port_fd;
}

int llwrite(int port_fd, uint8_t *data, int size) {
  init_timeout_handler();
  uint8_t byte;
  tries = 0;
  ctrl_state state = C_START;
  ctrl_state rec_state = C_START;

  information_frame info_frame = create_information_frame(seq_num, data, size);

  do {
    write(port_fd, info_frame.bytes, info_frame.size); 

    alarm(3);
    timeout = false;

    while (state != C_STOP && !timeout) {
      read(port_fd, &byte, 1);
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
      return size;
    } else {
      printf("Receiver rejected frame. Resending..\n");
      return llwrite(port_fd, data, size);
    }
  }

  printf("Max time-outs occurred: information frame or Receiver's confirmation was likely lost.\n");
  return -1;
}

int llread(int port_fd, uint8_t *data) { 
  init_timeout_handler(); 
  info_state state = I_START;
  info_state type_state = I_START;
  uint8_t byte;
  unsigned int size = 0;
  tries = 0;
  bool is_bbc2_ok = false;

  do {
    alarm(3);
    timeout = false;

    while (state != I_STOP && !timeout) {
      read(port_fd, &byte, 1);
      handle_information_frame_state(byte, seq_num, &state);

      // Accumulate data bytes
      if (state == I_DATA) {
        if (byte == FRAME_FLAG) {
          int real_size;
          uint8_t *destuff_data = destuff_bytes(data, size, &real_size);
          uint8_t bcc2 = destuff_data[(real_size)-1];
          size = real_size - 1;        // Remove BBC2 byte
          data = realloc(data, size); 
          
          for (int i = 0; i < size; i++) data[i] = destuff_data[i];

          if (bcc2 == frame_BCC2(data, size))
            state = I_BCC2_OK;
          else
            state = I_BBC2_NOT_OK;
        }
        else {
          data = realloc(data, ++(size));
          data[(size)-1] = byte;
        }
      }

      if (state == I_INFO_C_RCV || state == I_SET_C_RCV) {
        type_state = state;
      }
      else if (state == I_BCC2_OK || state == I_BBC2_NOT_OK){
        is_bbc2_ok = (state == I_BCC2_OK) ? true : false;
      }
    }
  } while (tries <= MAX_NR_TRIES && timeout);
             
  alarm(0);

  if (state != I_STOP) {
    return -1;
  }

  uint8_t frame[CTRL_FRAME_SIZE];

  if (type_state == I_INFO_C_RCV) {
    if (is_bbc2_ok) {
      create_control_frame(FRAME_CTRL_RR(seq_num), FRAME_ADDR_EM, frame);
      write(port_fd, frame, CTRL_FRAME_SIZE); 
      seq_num = 1 - seq_num;
      return size;
    }

    // Reject frame
    create_control_frame(FRAME_CTRL_REJ(seq_num), FRAME_ADDR_EM, frame);
    write(port_fd, frame, CTRL_FRAME_SIZE); 
    return llread(port_fd, data);
  }
  // If received another SET
  create_control_frame(FRAME_CTRL_UA, FRAME_ADDR_EM, frame);
  write(port_fd, frame, CTRL_FRAME_SIZE);
  return llread(port_fd, data);
}

int llclose(int port_fd, user_type type) { 
  init_timeout_handler();
  uint8_t byte = 0;
  uint8_t ctrl_frame[CTRL_FRAME_SIZE];
  ctrl_state state = C_START;
  switch (type) {
    case EMITTER: {
      tries = 0;

      create_control_frame(FRAME_CTRL_DISC, FRAME_ADDR_EM, ctrl_frame);

      do {

        write(port_fd, ctrl_frame, CTRL_FRAME_SIZE);
        printf("Termination of connection\n");
        alarm(5);

        timeout = false;

        while (state != C_STOP && !timeout) {
          read(port_fd, &byte, 1);
          handle_unnumbered_frame_state(byte, FRAME_CTRL_DISC, FRAME_ADDR_REC, &state);
        }

      } while (tries <= MAX_NR_TRIES && timeout);

      alarm(0);

      if (state != C_STOP) {
        printf("Failed: timeout");
        return -1;
      }

      create_control_frame(FRAME_CTRL_UA, FRAME_ADDR_REC, ctrl_frame);
      write(port_fd, ctrl_frame, CTRL_FRAME_SIZE);
      printf("Disconnected\n");
      break;
    }

    case RECEIVER: {
      // READ DISC
      while (state != C_STOP){
        read(port_fd, &byte, 1);
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
        handle_unnumbered_frame_state(byte, FRAME_CTRL_UA, FRAME_ADDR_REC, &state);
      }

      if (state != C_STOP) {
        printf("Failed: no UA received\n");
        return -1;
      }
      printf("Disconnected\n");
      break;
    }
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