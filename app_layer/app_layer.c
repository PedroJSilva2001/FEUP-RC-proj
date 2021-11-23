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


    free(data);
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

int read_control_packet(int fd, char *filename, unsigned long *length) {
    char *bytes = (char *) malloc(sizeof(char));
    unsigned long nr_bytes_read = llread(fd, bytes);

    // TODO
    unsigned long current_byte = 0;

    while (current_byte < nr_bytes_read){
        unsigned long size = bytes[2];
        
        switch (bytes[1]) {
            case PACKET_T_LENGTH:
                memcpy(length, &bytes[3], size);
                break;

            case PACKET_T_NAME: 
                filename = realloc(filename, size);
                memcpy(filename, &bytes[3], size);
                break;
        }

        current_byte++;
    }


    free(bytes);
    return 0;

}

int read_data_packets(int fd, char *filename, unsigned long size) {
    FILE *file;

    if ((file = fopen(filename, "wb")) == NULL) {
        printf("Not possible to create file!\n");
        return -1;
    }

    unsigned int current_sequence_nr = 0;
    unsigned long nr_bytes_read = 0;

    while (nr_bytes_read < size) {
        char *buffer;
        char data[PACKET_MAX_DATA_SIZE];

        nr_bytes_read += llread(fd, buffer);

        unsigned int seq = buffer[1];

        // TODO: IGNORAR SE FOR REPETIDO: SEQ < CURRENT ??
        if (seq != current_sequence_nr) {
            fclose(file);
            printf("Data packets not in order!\n");
            return -1;
        }

        unsigned int data_length = 256 * (unsigned int) buffer[2] + (unsigned int) buffer[3];  // Length = 256 * l2 + l1;

        memcpy(data, &buffer[4], data_length);
        fwrite(data, sizeof(char), data_length, file);

        current_sequence_nr = (current_sequence_nr + 1) % 256;
    }

    fclose(file);
    return 0;
}


int receive_file(int fd) {
    char *filename = (char *) malloc(sizeof(char));
    unsigned long size;
    
    if (read_control_packet(fd, filename, &size) < 0)
        return -1;

    if (read_data_packets(fd, filename, size) < 0) 
        return -1;
    
    free(filename);
    return 0;
}