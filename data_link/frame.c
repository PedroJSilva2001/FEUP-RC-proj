#include "frame.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BYTE_TRANSPARENCY(n) ((n) ^ 0x20)

static unsigned int stuff_bytes(char *info_frame, char *stuffed_info_frame, unsigned int length);
static char frame_BCC2(char *data_packet, unsigned int data_packet_size);

void create_control_frame(char control, char address, char *ctrl_frame) {
  ctrl_frame[0] = FRAME_FLAG;
  ctrl_frame[1] = address;
  ctrl_frame[2] = control;
  ctrl_frame[3] = FRAME_BCC1(address, control);
  ctrl_frame[4] = FRAME_FLAG;
}

unsigned int create_information_frame(char control, char address, char *data_packet, 
                                      unsigned int data_packet_size, char *info_frame) {
  unsigned int base_size = INFO_FRAME_MIN_SIZE + data_packet_size;
  char *base_frame = (char *) malloc(sizeof (char) * base_size);

  base_frame[0] = FRAME_FLAG;
  base_frame[1] = address;
  base_frame[2] = control;
  base_frame[3] = FRAME_BCC1(address, control);
  memcpy(&base_frame[4], data_packet, data_packet_size);
  base_frame[4+data_packet_size] = frame_BCC2(data_packet, data_packet_size);
  base_frame[5+data_packet_size] = FRAME_FLAG;

  unsigned int size = stuff_bytes(base_frame, info_frame, base_size);
  free(base_frame);

  return size;
}

/** @brief Creates a new array of the frame using the byte stuffing tecnique.
 *  @param info_frame Information frame.
 *  @param stuffed_info_frame Stuffed Information frame (after tecnique).
 *  @param length Size of the array info_frame.
 */
static unsigned int stuff_bytes(char *base, char *info_frame, unsigned int base_size) {
  unsigned int index = 1;
  char byte;
  unsigned int size = base_size;

  info_frame[0] = FRAME_FLAG;

  for (unsigned int i = 1; i < base_size - 1; i++) {
    byte = base[i];
    switch (byte) {
      case FRAME_FLAG:
      case FRAME_ESCAPE:
        info_frame = realloc(info_frame, ++size);
        info_frame[index++] = FRAME_ESCAPE;
        info_frame[index] = BYTE_TRANSPARENCY(byte);
        break;
      default:
        info_frame[index] = byte;
        break;
    }
    index++;
  }

  info_frame[index] = FRAME_FLAG;

  return size;
}


char *destuff_bytes(char *stuffed_info_frame, unsigned int length, unsigned int *real_length) {
  char *destuffed_frame = (char *) malloc(sizeof (char) * length);
  unsigned int index = 1;

  *real_length = length;
  destuffed_frame[0] = FRAME_FLAG;

  for (unsigned int i = 1; i < length-1; i++) {
    if (stuffed_info_frame[i] == FRAME_ESCAPE) {
      destuffed_frame = realloc(destuffed_frame, --(*real_length));
      destuffed_frame[index] = BYTE_TRANSPARENCY(stuffed_info_frame[i+1]);
      i++;
    } else {
      destuffed_frame[index] = stuffed_info_frame[i];
    }
    index++;
  }

  destuffed_frame[*real_length-1] = FRAME_FLAG;

  return destuffed_frame;
}

static char frame_BCC2(char *data_packet, unsigned int data_packet_size) {
  char BCC2 = data_packet[0];
  for (unsigned int i = 1; i < data_packet_size; i++) {
    BCC2 ^= data_packet[i];
  }
  return BCC2;
}

/*int main() {
  char packet[3] = {0x3, FRAME_FLAG, 0x2};
  char *info_frame = (char *) malloc(sizeof (char) * 9);;
  unsigned int size = create_information_frame(FRAME_CTRL_DISC, FRAME_ADDR_EM, packet, 3, info_frame);
  printf("size = %d\n", size);
  
 char packet[4] = {0x3, FRAME_ESCAPE, FRAME_FLAG, 0x2};
  char *info_frame = (char *) malloc(sizeof (char) * 10);
  unsigned int size = create_information_frame(FRAME_CTRL_DISC, FRAME_ADDR_EM, packet, 4, info_frame);
    printf("size = %d\n", size);
  for (size_t i = 0; i < size; i++)
  {
    printf("byte = %x \n", info_frame[i]);
  }
  
  unsigned int s2;
  char *dest = destuff_bytes(info_frame, 12, &s2);
  printf("size2 = %d\n", s2);
  for (size_t i = 0; i < s2; i++)
  {
    printf("byte = %x \n", dest[i]);
  }
  
  // flag A C bcc1 data bcc2 flag
  // data = 0x3 esc flag^0x20 0x2
}*/