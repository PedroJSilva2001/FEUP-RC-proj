#include "packet.h"

#include <stdlib.h>
#include <string.h>

packet create_control_packet(uint8_t control, uint8_t type, unsigned long file_size) {
    packet control_packet;
    control_packet.size = CTRL_PACKET_MIN_SIZE + sizeof(unsigned long);

    uint8_t *bytes = (uint8_t *) malloc(sizeof (uint8_t) * control_packet.size);

    bytes[0] = control;
    bytes[1] = type;
    bytes[2] = sizeof(unsigned long);

    memcpy(&bytes[3], &file_size, sizeof(unsigned long));
    control_packet.bytes = bytes;
    return control_packet;
}

void add_to_control_packet(uint8_t type, unsigned int size, char *data, packet *control_packet) {
    unsigned int old_packet_size = control_packet->size;
    control_packet->size += 2 + size;
    
    control_packet->bytes = realloc(control_packet->bytes, sizeof (uint8_t) * control_packet->size);

    control_packet->bytes[old_packet_size] = type;
    control_packet->bytes[old_packet_size + 1] = size;

    memcpy(&control_packet->bytes[old_packet_size + 2], data, size);
}

packet create_data_packet(unsigned int sequence_nr, uint8_t* data, unsigned int size) {
    packet data_packet;
    data_packet.size = PACKET_DATA_MIN_SIZE + size;
    uint8_t *bytes = (uint8_t *) malloc(sizeof(uint8_t) * data_packet.size);

    bytes[0] = PACKET_CTRL_DATA;
    bytes[1] = sequence_nr % 256;
    bytes[2] = size / 256;
    bytes[3] = size % 256;

    memcpy(&bytes[4], data, size);
    data_packet.bytes = bytes;
    return data_packet;
}