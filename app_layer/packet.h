#ifndef APP_LAYER_packet_H
#define APP_LAYER_packet_H

#include <stdint.h>
#include "../data_link/frame.h"

typedef container packet;

/* Control Packet */
#define PACKET_CTRL_START 0x02
#define PACKET_CTRL_END   0x03
#define PACKET_T_LENGTH   0x00
#define PACKET_T_NAME     0x01

#define CTRL_PACKET_MIN_SIZE 3

/* Data Packet */
#define PACKET_CTRL_DATA 0x01

#define PACKET_DATA_MIN_SIZE 4
#define PACKET_MAX_DATA_SIZE 1024


/** @brief Creates control packet.
 *  @param control Control field of the packet (2 - start; 3 - end).
 *  @param type Type field of the packet (0 - size of file; 1 - name of file).
 *  @param file_size Size of the file.
 *  @return Returns struct with the packet and its size.
 */
packet create_control_packet(uint8_t control, uint8_t type, unsigned long file_size);

/** @brief Appends information to the control packet (file name).
 *  @param type Type field of the packet (0 - size of file; 1 - name of file).
 *  @param size Size of the data array (size of file name).
 *  @param data Array with information to add to the control byte (file name).
 */
void add_to_control_packet(uint8_t type, unsigned int size, char *data, packet *control_packet);

/** @brief Creates data packet.
 *  @param sequence_nr Sequence number of the packet.
 *  @param data Array with information.
 *  @param size Size of the data array.
 *  @return Returns struct with the packet and its size.
 */
packet create_data_packet(unsigned int sequence_nr, uint8_t* data, unsigned int size); 

#endif