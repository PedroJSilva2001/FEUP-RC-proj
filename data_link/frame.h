#ifndef DATA_LINK_frame_H
#define DATA_LINK_frame_H

#include <stdint.h>

#define FRAME_FLAG       0x7E
#define FRAME_CTRL_SET   0x03
#define FRAME_CTRL_DISC  0x0B
#define FRAME_CTRL_UA    0x07
#define FRAME_CTRL_RR(r) (r==0 ? 0x05 : 0x85)  
#define FRAME_CTRL_REJ(r) (r==0 ? 0x01 : 0x81) 
#define FRAME_ADDR_EM    0x03       // Comandos enviados pelo Emissor e Respostas enviadas pelo Receptor
#define FRAME_ADDR_REC   0x01       // Comandos enviados pelo Receptor e Respostas enviadas pelo Emissor
#define FRAME_BCC1(addr, ctrl)  (addr ^ ctrl)  
#define FRAME_ESCAPE 0x7d
#define FRAME_CTRL_INFO(s) (s==0 ? 0x00 : 0x40)

#define CTRL_FRAME_SIZE 5
#define INFO_FRAME_MIN_SIZE 6


typedef struct {
    uint8_t *bytes;
    unsigned int size;
} container;

typedef container information_frame;

/** @brief Creates control frame (S and U).
 *  @param control Control byte.
 *  @param address Address byte.
 *  @param ctrl_frame Array that will contain the created frame.
 */
void create_control_frame(uint8_t control, uint8_t address, uint8_t *ctrl_frame);

/** @brief Creates information frame (I).
 *  @param sequence_number Sequence number.
 *  @param data Array with data to send.
 *  @param data_size Length of the data array.
 *  @return Returns struct with information frame and its length.
 */
information_frame create_information_frame(uint8_t sequence_number, uint8_t *data, unsigned int data_size);

/** @brief Creates a new array of the frame using the byte stuffing tecnique.
 *  @param base Information frame.
 *  @param base_size Size of the original information frame.
 *  @param info_frame_bytes Stuffed frame (after tecnique).
 *  @return Returns size of the stuffed frame.
 */
static unsigned int stuff_bytes(uint8_t *base, unsigned int base_size, uint8_t *info_frame_bytes);

/** @brief Creates a new array of the frame reversing the the byte stuffing tecnique.
 *  @param stuffed_info_frame Stuffed information frame.
 *  @param length Size of the stuffed information frame.
 *  @param real_length Size of the destuffed frame (after destuffing tecnique).
 *  @return Returns destuffed frame.
 */
uint8_t *destuff_bytes(uint8_t *stuffed_info_frame, unsigned int length, unsigned int *real_length);

/** @brief Calculates de BCC2 of an information frame.
 *  @param data_packet Information frame.
 *  @param data_packet_size Size of the frame.
 *  @return Returns the result BCC2.
 */
uint8_t frame_BCC2(uint8_t *data_frame, unsigned int data_frame_size);

#endif