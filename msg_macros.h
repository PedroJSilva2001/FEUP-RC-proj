#ifndef MSG_MACROS_HPP
#define MSG_MACROS_HPP

#define FRAME_FLAG       0x7E
#define FRAME_CTRL_SET   0x03
#define FRAME_CTRL_DISC
#define FRAME_CTRL_UA    0x07
#define FRAME_CTRL_RR
#define FRAME_CTRL_
#define FRAME_ADDR_EM    0x03       // Comandos enviados pelo Emissor e Respostas enviadas pelo Receptor
#define FRAME_ADDR_REC   0x01       // Comandos enviados pelo Receptor e Respostas enviadas pelo Emissor
#define FRAME_BCC1(A,C)  ((A)^(C))  


typedef enum {
    START,
    FLAG,
    A,
    C,
    BCC,
    STOP
    
} state_t;

#endif
