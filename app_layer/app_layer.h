#ifndef APP_LAYER_app_layer_H
#define APP_LAYER_app_layer_H

int send_control_packet(int fd, uint8_t *filename, unsigned long file_size, uint8_t control);
int send_data_packet(int fd, uint8_t *filename, unsigned long size);
int send_file(int fd, uint8_t *filename, unsigned long size);

unsigned int read_control_packet(int fd, char *filename) ;
int read_data_packets(int fd, uint8_t *filename, unsigned long size);
int receive_file(int fd);

#endif