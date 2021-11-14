#include "packet.h"
#include "../msg_macros.h"

void create_control_packet(uint8_t control, uint8_t address, uint8_t *ctrl_packet) {
  ctrl_packet[0] = FRAME_FLAG;
  ctrl_packet[1] = address;
  ctrl_packet[2] = control;
  ctrl_packet[3] = FRAME_BCC1(address, control);
  ctrl_packet[4] = FRAME_FLAG;
}

void create_information_packet(uint8_t control, uint8_t address, uint8_t *data, int data_length ,uint8_t *info_packet) {

  uint8_t *inf = (uint8_t *) malloc(sizeof(uint8_t) *(6+length)); // TODO: REVER
  info_packet[0] = FRAME_FLAG;
  info_packet[1] = address;
  info_packet[2] = control;
  info_packet[3] = FRAME_BCC1(address, control);
  
  
  
  // TODO: DATA
  // ...
  
  
  //info_packet[?] = ?
  //info_packet[?] = FRAME_FLAG;
  
  info_packet = inf;
}
