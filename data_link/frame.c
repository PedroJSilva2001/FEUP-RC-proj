#include "frame.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BYTE_TRANSPARENCY(n) ((n) ^ 0x20)

static unsigned int stuff_bytes(uint8_t *base, unsigned int base_size, uint8_t *info_frame_bytes);

void create_control_frame(uint8_t control, uint8_t address, uint8_t *ctrl_frame) {
  ctrl_frame[0] = FRAME_FLAG;
  ctrl_frame[1] = address;
  ctrl_frame[2] = control;
  ctrl_frame[3] = FRAME_BCC1(address, control);
  ctrl_frame[4] = FRAME_FLAG;
}

information_frame create_information_frame(uint8_t sequence_number, uint8_t *data, unsigned int data_size) {
  unsigned int base_size = INFO_FRAME_MIN_SIZE + data_size;
  uint8_t *base_frame = (uint8_t *) malloc(sizeof (uint8_t) * base_size);
  uint8_t *bytes = (uint8_t *) malloc(sizeof (uint8_t) * base_size);
  information_frame info_frame;

  base_frame[0] = FRAME_FLAG;
  base_frame[1] = FRAME_ADDR_EM;
  base_frame[2] = FRAME_CTRL_INFO(sequence_number);
  base_frame[3] = FRAME_BCC1(FRAME_ADDR_EM, FRAME_CTRL_INFO(sequence_number));
  memcpy(&base_frame[4], data, data_size);
  base_frame[4+data_size] = frame_BCC2(data, data_size);
  base_frame[5+data_size] = FRAME_FLAG;

  unsigned int size = stuff_bytes(base_frame, base_size, bytes);

  free(base_frame);

  info_frame.bytes = bytes;
  info_frame.size = size;

  return info_frame;
}

/** @brief Creates a new array of the frame using the byte stuffing tecnique.
 *  @param info_frame Information frame.
 *  @param stuffed_info_frame Stuffed Information frame (after tecnique).
 *  @param length Size of the array info_frame.
 */
static unsigned int stuff_bytes(uint8_t *base, unsigned int base_size, uint8_t *info_frame_bytes) {
  unsigned int index = 1;
  uint8_t byte;
  unsigned int size = base_size;

  info_frame_bytes[0] = FRAME_FLAG;

  for (unsigned int i = 1; i < base_size - 1; i++) {
    byte = base[i];
    switch (byte) {
      case FRAME_FLAG:
      case FRAME_ESCAPE:
        info_frame_bytes = realloc(info_frame_bytes, ++size);
        info_frame_bytes[index++] = FRAME_ESCAPE;
        info_frame_bytes[index] = BYTE_TRANSPARENCY(byte);
        break;
      default:
        info_frame_bytes[index] = byte;
        break;
    }
    index++;
  }

  info_frame_bytes[index] = FRAME_FLAG;

  return size;
}


uint8_t *destuff_bytes(uint8_t *stuffed_info_frame, unsigned int length, unsigned int *real_length) {
  uint8_t *destuffed_frame = (uint8_t *) malloc(sizeof (uint8_t) * length);
  unsigned int index = 0;

  *real_length = length;

  for (unsigned int i = 0; i < length; i++) {
    if (stuffed_info_frame[i] == FRAME_ESCAPE) {
      destuffed_frame = realloc(destuffed_frame, --(*real_length));
      destuffed_frame[index] = BYTE_TRANSPARENCY(stuffed_info_frame[i+1]);
      i++;
    } else {
      destuffed_frame[index] = stuffed_info_frame[i];
    }
    index++;
  }

  return destuffed_frame;
}

uint8_t frame_BCC2(uint8_t *data_packet, unsigned int data_packet_size) {
  uint8_t BCC2 = data_packet[0];
  for (unsigned int i = 1; i < data_packet_size; i++) {
    BCC2 ^= data_packet[i];
  }
  return BCC2;
}

/*int main() {
  uint8_t packet[3] = {0x3, FRAME_FLAG, 0x2};
  uint8_t *info_frame = (uint8_t *) malloc(sizeof (uint8_t) * 9);;
  unsigned int size = create_information_frame(FRAME_CTRL_DISC, FRAME_ADDR_EM, packet, 3, info_frame);
  printf("size = %d\n", size);
  
 uint8_t packet[4] = {0x3, FRAME_ESCAPE, FRAME_FLAG, 0x2};
  uint8_t *info_frame = (uint8_t *) malloc(sizeof (uint8_t) * 10);
  unsigned int size = create_information_frame(FRAME_CTRL_DISC, FRAME_ADDR_EM, packet, 4, info_frame);
    printf("size = %d\n", size);
  for (size_t i = 0; i < size; i++)
  {
    printf("byte = %x \n", info_frame[i]);
  }
  
  unsigned int s2;
  uint8_t *dest = destuff_bytes(info_frame, 12, &s2);
  printf("size2 = %d\n", s2);
  for (size_t i = 0; i < s2; i++)
  {
    printf("byte = %x \n", dest[i]);
  }
  
  // flag A C bcc1 data bcc2 flag
  // data = 0x3 esc flag^0x20 0x2
}*/
