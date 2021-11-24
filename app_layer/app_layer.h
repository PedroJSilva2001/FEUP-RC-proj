#ifndef APP_LAYER_app_layer_H
#define APP_LAYER_app_layer_H

int send_control_packet(int fd, char *filename, unsigned long file_size, char control);
int send_data_packet(int fd, char *filename, unsigned long size);
int send_file(int fd, char *filename, unsigned long size);

int read_control_packet(int fd, char *filename, unsigned long *length);
int read_data_packets(int fd, char *filename, unsigned long size) ;
int receive_file(int fd);

#endif