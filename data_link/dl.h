#ifndef DATA_LINK_DL_H
#define DATA_LINK_DL_H

typedef enum {
    RECEIVER,
    EMITTER,
} user_type_t;

typedef struct termios termios_t; 

int llopen(user_type_t type, char *serial_port);

int llread();

int llwrite();

int llclose(int port_fd, user_type_t type);

#endif