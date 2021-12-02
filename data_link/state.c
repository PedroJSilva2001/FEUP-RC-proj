#include "state.h"

#include <stdlib.h>
#include "frame.h"

void handle_unnumbered_frame_state(uint8_t byte, uint8_t control, uint8_t address, ctrl_state *state) {
  switch (*state) {
    case C_START:
      if (byte == FRAME_FLAG) {
        *state = C_FLAG_RCV; 
      } break;

    case C_FLAG_RCV:
      if (byte == address)  {
        *state = C_A_RCV;
      } else if (byte == FRAME_FLAG) {
        *state = C_FLAG_RCV;
      } else {
        *state = C_START; 
      } break;

    case C_A_RCV:
      if (byte == control) {
        *state = C_C_RCV;
      } else if (byte == FRAME_FLAG) {
        *state = C_FLAG_RCV;
      } else {
        *state = C_START; 
      } break;

    case C_C_RCV:
      if (byte == address ^ control) {
        *state = C_BCC_OK;
      } else if (byte == FRAME_FLAG) {
        *state = C_FLAG_RCV;
      } else {
        *state = C_START; 
      } break;

    case C_BCC_OK:
      if (byte == FRAME_FLAG) {
        *state = C_STOP;
      } else {
        *state = C_START; 
      } break;
  }
}

void handle_information_frame_state(uint8_t byte, uint8_t s, info_state *state) {
  switch (*state) {
    case I_START:
      if (byte == FRAME_FLAG) {
        *state = I_FLAG_RCV;
      } break;

    case I_FLAG_RCV:
      if (byte == FRAME_ADDR_EM) { 
        *state = I_A_RCV;
      } else if (byte == FRAME_FLAG) {
        *state = I_FLAG_RCV;
      } else {
        *state = I_START;
      } break;

    case I_A_RCV:
      if (byte == FRAME_CTRL_INFO(s)) {
        *state = I_INFO_C_RCV;
      } else if (byte == FRAME_CTRL_SET) {
        *state = I_SET_C_RCV;
      } else if (byte == FRAME_FLAG) {
        *state = I_FLAG_RCV;
      } else {
        *state = I_START;
      } break;

    case I_INFO_C_RCV:
      if (byte == FRAME_ADDR_EM ^ FRAME_CTRL_INFO(s)) {
        *state = I_INFO_BCC1_OK;
      } else if (byte == FRAME_FLAG) {
        *state = I_FLAG_RCV;
      } else {
        *state = I_START; 
      } break;

    case I_SET_C_RCV:
      if (byte == FRAME_ADDR_EM ^ FRAME_CTRL_SET) {
        *state = I_SET_BCC1_OK;
      } else if (byte == FRAME_FLAG) {
        *state = I_FLAG_RCV;
      } else {
        *state = I_START; 
      } break;

    case I_SET_BCC1_OK:
      if (byte == FRAME_FLAG) {
        *state = I_STOP;
      } else {
        *state = I_START; 
      } break;

    case I_INFO_BCC1_OK:
      *state = I_DATA;
    break;

    case I_DATA:
    break;
  }
}

void handle_supervision_frame_state(uint8_t byte, uint8_t r, ctrl_state *state) {
  switch (*state) {
    case C_START:
      if (byte == FRAME_FLAG) {
        *state = C_FLAG_RCV;
      } break;
    
    case C_FLAG_RCV:
      if (byte == FRAME_ADDR_EM) {
        *state = C_A_RCV;
      } else if (byte == FRAME_FLAG) { 
        *state = C_FLAG_RCV;
      } else {
        *state = C_START;
      } break;

    case C_A_RCV:
      if (byte == FRAME_CTRL_REJ(r)) {
        *state = C_REJ_RCV;
      } else if (byte == FRAME_CTRL_RR(r)) {
        *state = C_RR_RCV; 
      } else if (byte == FRAME_FLAG) {
        *state = C_FLAG_RCV;
      } else {
        *state = C_START;
      } break;

    case C_REJ_RCV:
      if (byte == FRAME_ADDR_EM ^ FRAME_CTRL_REJ(r)) {
        *state = C_BCC_OK;
      } else if (byte == FRAME_FLAG) {
        *state = C_FLAG_RCV;
      } else {
        *state = C_START; 
      } break;

    case C_RR_RCV:
      if (byte == FRAME_ADDR_EM ^ FRAME_CTRL_RR(r)) {
        *state = C_BCC_OK;
      } else if (byte == FRAME_FLAG) {
        *state = C_FLAG_RCV;
      } else {
        *state = C_START; 
      } break;

    case C_BCC_OK:
      if (byte == FRAME_FLAG) {
        *state = C_STOP;
      } else {
        *state = C_START; 
      } break;
  }
}