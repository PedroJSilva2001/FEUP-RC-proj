#ifndef DATA_LINK_frame_H
#define DATA_LINK_frame_H

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

#define CTRL_FRAME_SIZE 5
#define INFO_FRAME_MIN_SIZE 6

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
unsigned int create_information_frame(char control, char address, char *data, unsigned int data_length, char *info_frame);


char *destuff_bytes(char *stuffed_info_frame, unsigned int length, unsigned int *real_length);

char frame_BCC2(char *data_packet, unsigned int data_packet_size);

#endif