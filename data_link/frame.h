#ifndef DATA_LINK_frame_H
#define DATA_LINK_frame_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../msg_macros.h"

/** @brief Creates control frame (S and U).
 *  @param control Control byte.
 *  @param address Address byte.
 *  @param ctrl_frame Array that will contain the created frame.
 */
void create_control_frame(char control, char address, char *ctrl_frame);

/** @brief Creates information frame (I).
 *  @param control Control byte.
 *  @param address Address byte.
 *  @param data Array with data to send.
 *  @param data_length Length of the data array.
 *  @param info_frame Array that will contain the created frame.
 */
void create_information_frame(char control, char address, char *data, int data_length, char *info_frame);

/** @brief Creates a new array of the frame using the byte stuffing tecnique.
 *  @param info_frame Information frame.
 *  @param stuffed_info_frame Stuffed Information frame (after tecnique).
 *  @param length Size of the array info_frame.
 */
void stuffing(char *info_frame, char *stuffed_info_frame, unsigned int length);

#endif
