#ifndef DATA_LINK_DL_H
#define DATA_LINK_DL_H

#include <stdint.h>

/**
 * Type of user (Receiver or Emitter).
 */
typedef enum {
    RECEIVER,
    EMITTER,
} user_type;

typedef struct termios termios; 

int llopen(uint8_t serial_port[], user_type type);

int llwrite(int port_fd, uint8_t *data, int size);

int llread(int port_fd, uint8_t *data);

int llclose(int port_fd, user_type type);

#endif