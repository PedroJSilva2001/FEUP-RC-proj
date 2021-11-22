#include "packet.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../data_link/dl.h"

int send_control_packet(int fd, char *filename, unsigned long file_size, char control) {
    char *ctrl_packet;
    unsigned int packet_length;
    char *size_buf = (char *) &file_size;

    create_control_packet(control, PACKET_T_LENGTH, sizeof (unsigned long), size_buf, ctrl_packet, &packet_length); 
    add_to_control_packet(PACKET_T_NAME, strlen(filename), filename, ctrl_packet, &packet_length);

    if (llwrite(fd, ctrl_packet, packet_length) < 0) {
        printf("Not possible to send control app packet!\n");
        return -1;
    }
    return 0;
}

int send_data_packet(int fd, char *filename, unsigned long size) {
    FILE *file;

    if ((file = fopen(filename, "rb")) == NULL) {
        printf("Not possible to open file!\n");
        return -1;
    }

    char *data = (char *) malloc(sizeof(char) * PACKET_MAX_DATA_SIZE);
    unsigned int sequence_nr = 0;
    size_t bytes_read;

    char *data_packet;
    unsigned int packet_length;
    unsigned int size_to_read = (size > PACKET_MAX_DATA_SIZE) ? PACKET_MAX_DATA_SIZE : size;

    while (size > 0) {
        bytes_read = fread(data, sizeof(char), size_to_read, file);
        create_data_packet(sequence_nr, data, bytes_read, data_packet, &packet_length);

        size -= bytes_read;
        size_to_read = (size > PACKET_MAX_DATA_SIZE) ? PACKET_MAX_DATA_SIZE : size;

        if (llwrite(fd, data_packet, packet_length) < 0) {
            printf("Not possible to send data app packet!\n");
            return -1;
        }

        sequence_nr++;
    }

    return 0;
}

int send_file(int fd, char *filename, unsigned long size) {

    if (send_control_packet(fd, filename, size, PACKET_CTRL_START) < 0)
        return -1;


    if (send_data_packet(fd, filename, size) < 0)
        return -1;


    if (send_control_packet(fd, filename, size, PACKET_CTRL_END) < 0)
        return -1;


    printf("Application packets sent\n");
    return 0;
}