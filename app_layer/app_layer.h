#ifndef APP_LAYER_app_layer_H
#define APP_LAYER_app_layer_H

#include <stdint.h>

/* Emitter*/

/** @brief Create and send control packet.
 *  @param fd File descriptor to send packet to.
 *  @param filename Name of the file to send.
 *  @param file_size Size of the file to send.
 *  @param control Control field of the packet (2 - start; 3 - end).
 *  @return Returns 0 upon success, -1 otherwise.
 */
int send_control_packet(int fd, char *filename, unsigned long file_size, uint8_t control);

/** @brief Create and send data packets.
 *  @param fd File descriptor to send packet to.
 *  @param filename Name of the file to send.
 *  @param size Size of the file to send.
 *  @return Returns 0 upon success, -1 otherwise.
 */
int send_data_packet(int fd, uint8_t *filename, unsigned long size);

/** @brief Sends file.
 *  @param fd File descriptor to send packet to.
 *  @param filename Name of the file to send.
 *  @param size Size of the file to send.
 *  @return Returns 0 upon success, -1 otherwise.
 */
int send_file(int fd, uint8_t *filename, unsigned long size);


/* Receiver */

/** @brief Reads and parses control packet.
 *  @param fd File descriptor to receive packet from.
 *  @param filename Name of the file to receive.
 *  @return Returns size of the file to receive. 0 in case size exceeded 255 bytes.
 */
unsigned int read_control_packet(int fd, char *filename);

/** @brief Reads and parses a single data packet.
 *  @param fd File descriptor to receive packet from.
 *  @param data Buffer with the data in the packet.
 *  @param size Length of the data in the packet.
 *  @param seq Sequence number of the packet.
 *  @return Returns number of bytes read.
 */
int read_single_data_packet(int fd, uint8_t *data, unsigned long *size, uint8_t *seq);

/** @brief Receives and parses data packets.
 *  @param fd File descriptor to receive packet from.
 *  @param filename Name of the file to receive.
 *  @param size Size of the file to receive.
 *  @return Returns 0 upon success, -1 otherwise.
 */
int read_data_packets(int fd, uint8_t *filename, unsigned long size);

/** @brief Receives file.
 *  @param fd File descriptor to receive file from.
 *  @return Returns 0 upon success, -1 otherwise.
 */
int receive_file(int fd);

#endif