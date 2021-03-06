#ifndef DATA_LINK_STATE_H
#define DATA_LINK_STATE_H

#include <stdint.h>

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
    I_SET_A_RCV,
    I_INFO_BCC1_OK,
    I_SET_BCC1_OK,
    I_DATA,
    I_BCC2_OK,
    I_BBC2_NOT_OK,
    I_STOP,
    I_INFO_C_RCV,
    I_SET_C_RCV,
} info_state;

/** @brief Checks received unnumbered frame (SET, UA and DISC) byte and updates the state machine.
 *  @param ctrl_byte Byte to check.
 *  @param control Control byte.
 *  @param address Address byte.
 *  @param state Current state of the state machine.
 */
void handle_unnumbered_frame_state(uint8_t ctrl_byte, uint8_t control, uint8_t address, ctrl_state *state);

/** @brief Checks received information frame byte and updates the state machine.
 *  @param byte Byte to check.
 *  @param s Sequence number.
 *  @param state Current state of the state machine.
 */
void handle_information_frame_state(uint8_t byte, uint8_t s, info_state *state);

/** @brief Checks received supervision frame (RR and REJ) byte and updates the state machine.
 *  @param byte Byte to check.
 *  @param r Sequence number.
 *  @param state Current state of the state machine.
 */
void handle_supervision_frame_state(uint8_t byte, uint8_t r, ctrl_state *state);

#endif