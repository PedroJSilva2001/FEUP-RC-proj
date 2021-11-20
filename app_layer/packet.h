#ifndef APP_LAYER_packet_H
#define APP_LAYER_packet_H

/* Control Packet */
#define PACKET_CTRL_START 0x02
#define PACKET_CTRL_END   0x03
#define PACKET_T_LENGTH   0x00
#define PACKET_T_NAME     0x01

#define CTRL_PACKET_MIN_SIZE 3


/* Data Packet */
#define PACKET_CTRL_DATA 0x01

#define PACKET_DATA_MIN_SIZE 4


#endif