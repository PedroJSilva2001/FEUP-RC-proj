#include "packet.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>

int send_control_packet(int fd, char *filename, unsigned long size, char control) {
    char *ctrl_packet;
    unsigned int packet_length;
    //create_control_packet(control, PACKET_T_LENGTH, size, ??? ,ctrl_packet,&packet_length); DÚVIDA??
    
    add_to_control_packet(PACKET_T_NAME, strlen(filename), filename, ctrl_packet, &packet_length);

    if (write(fd, ctrl_packet, packet_length) < 0) {
        printf("Not possible to send control app packet!\n");
        return -1;
    }
    return 0;
}

int send_data_packet(int fd, char *filename, unsigned long size) {
    // TODO


    return 0;
}

int send_file(int fd, char *filename, unsigned long size) {

    if (send_control_packet(fd, filename, size, PACKET_CTRL_START) < 0)
        return -1;


    // TODO: DATA
    if (send_data_packet(fd, filename, size) < 0)
        return -1;


    if (send_control_packet(fd, filename, size, PACKET_CTRL_END) < 0)
        return -1;


    printf("Application packets sent\n");
    return 0;
}