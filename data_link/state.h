#ifndef DATA_LINK_STATE_H
#define DATA_LINK_STATE_H

/**
 * State machine for the control frame.
 */
typedef enum {
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    STOP
} msg_state;

/**
 * State machine for the information frame.
 */
typedef enum {
    ISTART,
    IFLAG_RCV,
    IA_RCV,
    IC_RCV,
    IBCC1_OK,
    IDATA,
    IBCC2_OK,
    ISTOP
} info_state;

/** @brief Checks received control byte and updates the state machine.
 *  @param ctrl_byte Byte to check.
 *  @param control Control byte.
 *  @param address Address byte.
 *  @param state Current state of the state machine.
 */
void check_control_frame_byte(char ctrl_byte, char control, char address, msg_state *state);

void check_information_frame_byte(char byte, char control, char address, info_state *state, char *data, unsigned int *size);

#endif