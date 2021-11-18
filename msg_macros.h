#ifndef MSG_MACROS_H
#define MSG_MACROS_H

#define FRAME_FLAG       0x7E
#define FRAME_CTRL_SET   0x03
#define FRAME_CTRL_DISC  0x0B
#define FRAME_CTRL_UA    0x07
#define FRAME_CTRL_RR(r) (r==0 ? 0x05 : 0x85)  
#define FRAME_CTRL_REJ(r) (r==0 ? 0x01 : 0x81) 
#define FRAME_ADDR_EM    0x03       // Comandos enviados pelo Emissor e Respostas enviadas pelo Receptor
#define FRAME_ADDR_REC   0x01       // Comandos enviados pelo Receptor e Respostas enviadas pelo Emissor
#define FRAME_BCC1(addr, ctrl)  (addr, ctrl)  
#define CTRL_PACKET_SIZE 5

#define ESCAPE 0x7d

#endif
