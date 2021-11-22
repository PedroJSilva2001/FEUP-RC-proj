#ifndef DATA_LINK_STATE_H
#define DATA_LINK_STATE_H

/**
 * State machine for the control frame.
 */
typedef enum {
    C_START,
    C_FLAG_RCV,
    C_A_RCV,
    C_C_RCV,
    C_BCC_OK,
    C_STOP,
    C_RR_RCV,
    C_REJ_RCV,
} ctrl_state;

/**
 * State machine for the information frame.
 */
typedef enum {
    I_START,
    I_FLAG_RCV,
    I_A_RCV,
    I_C_RCV,
    I_BCC1_OK,
    I_DATA,
    I_BCC2_OK,
    I_STOP
} info_state;

/** @brief Checks received control byte and updates the state machine.
 *  @param ctrl_byte Byte to check.
 *  @param control Control byte.
 *  @param address Address byte.
 *  @param state Current state of the state machine.
 */
void handle_unnumbered_frame_state(char ctrl_byte, char control, char address, ctrl_state *state);

void handle_information_frame_state(char byte, char control, char address, info_state *state, char *data, unsigned int *size);

void handle_supervision_frame_state(char byte, char control, char address, ctrl_state *state);

#endif