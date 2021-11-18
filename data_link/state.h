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

/** @brief Checks received control byte and updates the state machine.
 *  @param ctrl_byte Byte to check.
 *  @param control Control byte.
 *  @param address Address byte.
 *  @param state Current state of the state machine.
 */
void check_control_frame_byte(char ctrl_byte, char control, char address, msg_state *state);

#endif