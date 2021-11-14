#ifndef DATA_LINK_DL_H
#define DATA_LINK_DL_H

typedef enum {
    RECEIVER,
    EMITTER,
} user_type_t;

typedef struct termios termios_t; 

int llopen(char *serial_port, user_type_t type);

int llwrite(int fd, char *buffer, int length):

int llread(int fd, char *buffer);

int llclose(int port_fd, user_type_t type);

#endif
