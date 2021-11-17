#ifndef DATA_LINK_PACKET_H
#define DATA_LINK_PACKET_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../msg_macros.h"

#define INFO_PACKET_MIN_SIZE 6

/** @brief Creates control packet (S and U).
 *  @param control Control byte.
 *  @param address Address byte.
 *  @param ctrl_packet Array that will contain the created packet.
 */
void create_control_packet(uint8_t control, uint8_t address, uint8_t *ctrl_packet);

/** @brief Creates information packet (I).
 *  @param control Control byte.
 *  @param address Address byte.
 *  @param data Array with data to send.
 *  @param data_length Length of the data array.
 *  @param info_packet Array that will contain the created packet.
 */
void create_information_packet(uint8_t control, uint8_t address, uint8_t *data, int data_length ,uint8_t *info_packet);

/** @brief Creates a new array of the packet using the byte stuffing tecnique.
 *  @param info_packet Information packet.
 *  @param stuffed_info_packet Stuffed Information packet (after tecnique).
 *  @param length Size of the array info_packet.
 */
void stuffing(uint8_t *info_packet, uint8_t *stuffed_info_packet, unsigned int length);

#endif
