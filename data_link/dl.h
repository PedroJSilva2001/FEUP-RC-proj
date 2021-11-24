#ifndef DATA_LINK_DL_H
#define DATA_LINK_DL_H

/**
 * Type of user (Receiver or Emitter).
 */
typedef enum {
    RECEIVER,
    EMITTER,
} user_type;

typedef struct termios termios; 

int llopen(int com, user_type type);

int llwrite(int port_fd, char *data, int size);

int llread(int port_fd, char *data);

int llclose(int port_fd, user_type type);

#endif