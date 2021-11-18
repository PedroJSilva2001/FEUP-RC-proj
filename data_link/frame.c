#include "frame.h"
#include "../msg_macros.h"

#define INFO_MSG_MIN_SIZE 6

void create_control_frame(char control, char address, char *ctrl_frame) {
  ctrl_frame[0] = FRAME_FLAG;
  ctrl_frame[1] = address;
  ctrl_frame[2] = control;
  ctrl_frame[3] = FRAME_BCC1(address, control);
  ctrl_frame[4] = FRAME_FLAG;
}

void create_information_frame(char control, char address, char *data, int data_length, char *info_frame) {

  char *inf = (char *) malloc(sizeof(char) *(INFO_MSG_MIN_SIZE + data_length)); // TODO: REVER
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

  stuffing(inf, info_frame, INFO_MSG_MIN_SIZE + data_length);
}


void stuffing(char *info_frame, char *stuffed_info_frame, unsigned int length) {
  // TODO: REVER
  unsigned int index = 1;

  for (unsigned int i = 1; i < length - 1; i++) {
    if (info_frame[i] == FRAME_FLAG) {

      stuffed_info_frame = realloc(stuffed_info_frame, length);
      length++;
      
      stuffed_info_frame[index] = ESCAPE;
      index++;
      stuffed_info_frame[index] = FRAME_FLAG ^ 0x20;

    } else if (info_frame[i] == ESCAPE) {

      stuffed_info_frame = realloc(stuffed_info_frame, length);
      length++;
      
      stuffed_info_frame[index] = ESCAPE;
      index++;
      stuffed_info_frame[index] = ESCAPE ^ 0x20;

    } else {

      stuffed_info_frame[index] = info_frame[i];
    }

    index++;
  }

  stuffed_info_frame[index] = FRAME_FLAG;
}