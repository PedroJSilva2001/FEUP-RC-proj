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

int llopen(char *serial_port, user_type type);

int llwrite(int fd, char *buffer, int length);

int llread(int fd, char *buffer);

int llclose(int port_fd, user_type type);

#endif
