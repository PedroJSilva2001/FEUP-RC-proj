#include "packet.h"
#include "../msg_macros.h"

void create_control_packet(uint8_t control, uint8_t address, uint8_t *ctrl_packet) {
  ctrl_packet[0] = FRAME_FLAG;
  ctrl_packet[1] = address;
  ctrl_packet[2] = control;
  ctrl_packet[3] = FRAME_BCC1(address, control);
  ctrl_packet[4] = FRAME_FLAG;
}