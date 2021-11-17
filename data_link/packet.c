#include "packet.h"
#include "../msg_macros.h"

void create_control_packet(char control, char address, char *ctrl_packet) {
  ctrl_packet[0] = FRAME_FLAG;
  ctrl_packet[1] = address;
  ctrl_packet[2] = control;
  ctrl_packet[3] = FRAME_BCC1(address, control);
  ctrl_packet[4] = FRAME_FLAG;
}

void create_information_packet(char control, char address, char *data, int data_length, char *info_packet) {

  char *inf = (char *) malloc(sizeof(char) *(INFO_PACKET_MIN_SIZE + data_length)); // TODO: REVER
  inf[0] = FRAME_FLAG;
  inf[1] = address;
  inf[2] = control;
  inf[3] = FRAME_BCC1(address, control);
  
  // TODO: DATA
  for (int i = 0; i < data_length; i++) {
    inf[4 + i] = data[i];
  }

  // TODO: BBC2
  //inf[4 + data_length] = BBC2
  inf[5 + data_length] = FRAME_FLAG;

  stuffing(inf, info_packet, INFO_PACKET_MIN_SIZE + data_length);
}


void stuffing(char *info_packet, char *stuffed_info_packet, unsigned int length) {
  // TODO: REVER
  unsigned int index = 1;

  for (unsigned int i = 1; i < length - 1; i++) {
    if (info_packet[i] == FRAME_FLAG) {

      stuffed_info_packet = realloc(stuffed_info_packet, length);
      length++;
      
      stuffed_info_packet[index] = ESCAPE;
      index++;
      stuffed_info_packet[index] = FRAME_FLAG ^ 0x20;

    } else if (info_packet[i] == ESCAPE) {

      stuffed_info_packet = realloc(stuffed_info_packet, length);
      length++;
      
      stuffed_info_packet[index] = ESCAPE;
      index++;
      stuffed_info_packet[index] = ESCAPE ^ 0x20;

    } else {

      stuffed_info_packet[index] = info_packet[i];
    }

    index++;
  }

  stuffed_info_packet[index] = FRAME_FLAG;
}