#include "app_layer.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "packet.h"
#include "../data_link/dl.h"

int send_control_packet(int fd, char *filename, unsigned long file_size, uint8_t control) {
    uint8_t *ctrl_packet = (uint8_t *) malloc(sizeof (uint8_t) * (CTRL_PACKET_MIN_SIZE + sizeof (unsigned long)));

    packet control_packet = create_control_packet(control, PACKET_T_LENGTH, file_size); 
    add_to_control_packet(PACKET_T_NAME, strlen(filename), filename, &control_packet);

    if (llwrite(fd,  control_packet.bytes,  control_packet.size) < 0) {
        printf("Not possible to send control app packet!\n");
        return -1;
    }

    return 0;
}

int send_data_packet(int fd, uint8_t *filename, unsigned long size) {
    FILE *file;

    if ((file = fopen(filename, "rb")) == NULL) {
        printf("Not possible to open file!\n");
        return -1;
    }

    uint8_t *data = (uint8_t *) malloc(sizeof(uint8_t) * PACKET_MAX_DATA_SIZE);
    unsigned int sequence_nr = 0;
    size_t bytes_read;

    unsigned int size_to_read = (size > PACKET_MAX_DATA_SIZE) ? PACKET_MAX_DATA_SIZE : size;

    while (size > 0) {
        bytes_read = fread(data, sizeof(uint8_t), size_to_read, file);
        packet data_packet = create_data_packet(sequence_nr, data, bytes_read);

        size -= bytes_read;
        size_to_read = (size > PACKET_MAX_DATA_SIZE) ? PACKET_MAX_DATA_SIZE : size;

        if (llwrite(fd, data_packet.bytes, data_packet.size) < 0) {
            printf("Not possible to send data app packet!\n");
            return -1;
        }
        sequence_nr++;
    }

    free(data);
    return 0;
}

int send_file(int fd, uint8_t *filename, unsigned long size) {
    printf("Sending start control packets...\n");
    if (send_control_packet(fd, filename, size, PACKET_CTRL_START) < 0)
        return -1;

    printf("Sending data packets...\n");
    if (send_data_packet(fd, filename, size) < 0)
        return -1;

    printf("Sending end control packets...\n");
    if (send_control_packet(fd, filename, size, PACKET_CTRL_END) < 0)
        return -1;

    printf("Application packets sent\n");
    return 0;
}

unsigned int read_control_packet(int fd, char *filename) {
    uint8_t *bytes = (uint8_t *) malloc(sizeof(uint8_t));
    unsigned int file_size;
    unsigned int bytes_read = llread(fd, bytes);

    unsigned int current_index = 1; // Byte 0 = control

    while (current_index != bytes_read) {
        char type = bytes[current_index++];
        char length = bytes[current_index++];

        switch (type) {
            case PACKET_T_LENGTH: {
                char file_length[length];
                memcpy(file_length, &bytes[current_index], length);
                file_size = *((unsigned int *) &file_length);
                
            break;
            }
        
            case PACKET_T_NAME: {
                if (length > 255) {
                    printf("Size exceeded\n");
                    return 0;
                }
                filename = realloc(filename, length);
                strncpy(filename, &bytes[current_index], length);

            break;
            }
        }
        current_index += length;
    }
    
    free(bytes);
    return file_size;
}

int read_single_data_packet(int fd, uint8_t *data, unsigned long *size, uint8_t *seq) {

    int bytes_read = llread(fd, data);
    *size = 256 * (unsigned int) data[2] + (unsigned int) data[3];  // Length = 256 * l2 + l1;
    *seq = data[1];
    memcpy(data, &(data[4]), *size);

    return bytes_read;
}

int read_data_packets(int fd, uint8_t *filename, unsigned long size) {
    FILE *file;

    if ((file = fopen("new.gif", "wb")) == NULL) {
        printf("Not possible to create file!\n");
        return -1;
    }

    unsigned int current_sequence_nr = 0;
    unsigned long nr_bytes_read = 0;
    unsigned long bytes_read;


    while (nr_bytes_read < size) {
        uint8_t data[PACKET_MAX_DATA_SIZE];
        uint8_t seq;
        if (read_single_data_packet(fd, data, &bytes_read, &seq) < 0) {
            printf("Error in llread: read_data_packets\n");
            return -1;
        }

        if (seq < current_sequence_nr) {
            printf("Repeated data packets! Ignored\n");
            continue;
        }
        else if (seq > current_sequence_nr) {
            fclose(file);
            printf("Data packets not in order!\n");
            return -1;
        }
        
        fwrite(data, sizeof(uint8_t), bytes_read, file);

        current_sequence_nr = (current_sequence_nr + 1) % 256;
        nr_bytes_read += bytes_read;
    }

    fclose(file);
    return 0;
}

int receive_file(int fd) {
    
    printf("Reading start control packet...\n");
    uint8_t *filename = (uint8_t *) malloc(sizeof(uint8_t));
    unsigned int size = read_control_packet(fd, filename);

    printf("Filename = %s\tSize of file = %d\n", filename, size);

    printf("Reading data packets...\n");
    if (read_data_packets(fd, filename, size) < 0) 
        return -1;
    
    printf("Reading end control packet...\n");
    size = read_control_packet(fd, filename);

    printf("Application end packet read\n");
    return 0;
}
