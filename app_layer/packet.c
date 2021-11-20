#include "packet.h"

#include <stdlib.h>
#include <string.h>

void create_control_packet(char control, char type, unsigned int size, char *ctrl_packet, char *data, unsigned int *packet_length) {

    *packet_length = CTRL_PACKET_MIN_SIZE + size;
    ctrl_packet = (char *) malloc(sizeof (char) * (*packet_length));

    ctrl_packet[0] = control;
    ctrl_packet[1] = type;
    ctrl_packet[2] = size;

    memcpy(&ctrl_packet[3], data, size);
}

void create_data_packet(unsigned int sequence_nr, char* data, unsigned int size, char *data_packet, unsigned int *packet_length) {

    *packet_length = PACKET_DATA_MIN_SIZE + size;
    data_packet = (char *) malloc(sizeof(char) * (*packet_length));

    data_packet[0] = PACKET_CTRL_DATA;
    data_packet[1] = sequence_nr % 256;
    data_packet[2] = size / 256;
    data_packet[3] = size % 256;

    memcpy(&data_packet[4], data, size);
}