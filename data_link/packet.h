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
void create_control_packet(char control, char address, char *ctrl_packet);

/** @brief Creates information packet (I).
 *  @param control Control byte.
 *  @param address Address byte.
 *  @param data Array with data to send.
 *  @param data_length Length of the data array.
 *  @param info_packet Array that will contain the created packet.
 */
void create_information_packet(char control, char address, char *data, int data_length ,char *info_packet);

/** @brief Creates a new array of the packet using the byte stuffing tecnique.
 *  @param info_packet Information packet.
 *  @param stuffed_info_packet Stuffed Information packet (after tecnique).
 *  @param length Size of the array info_packet.
 */
void stuffing(char *info_packet, char *stuffed_info_packet, unsigned int length);

#endif
