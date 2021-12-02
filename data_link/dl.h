#ifndef DATA_LINK_DL_H
#define DATA_LINK_DL_H

#include <stdint.h>

/**
 * Type of user (Receiver or Emitter).
 */
typedef enum {
    RECEIVER,
    EMITTER,
} user_type;

typedef struct termios termios; 

/** @brief Establishes the data link of the serial port.
 *  @param serial_port Serial port.
 *  @param type Emitter or Receiver.
 *  @return Returns 0 upon success, -1 otherwise.
 */
int llopen(uint8_t serial_port[], user_type type);

/** @brief Writes to serial port.
 *  @param port_fd File descriptor of the serial port.
 *  @param data Information to write to port_fd.
 *  @param size Size of the data to write.
 *  @return Returns number of bytes written. -1 upon failure.
 */
int llwrite(int port_fd, uint8_t *data, int size);

/** @brief Reads of the serial port.
 *  @param port_fd File descriptor of the serial port.
 *  @param data Information to write to port_fd.
 *  @return Returns number of bytes read. -1 upon failure.
 */
int llread(int port_fd, uint8_t *data);

/** @brief Closes the data link of the serial port.
 *  @param serial_port Serial port.
 *  @param type Emitter or Receiver.
 *  @return Returns 0 upon success, -1 otherwise.
 */
int llclose(int port_fd, user_type type);

#endif
