# -*- Makefile -*-
all: clean emitter receiver

emitter:
	gcc -o "emitter" ./data_link/frame.c ./data_link/dl.c ./data_link/state.c ./app_layer/app_layer.c ./app_layer/packet.c emitter.c 

receiver:
	gcc -o "receiver" ./data_link/frame.c ./data_link/dl.c ./data_link/state.c ./app_layer/app_layer.c ./app_layer/packet.c receiver.c

clean:
	rm -f emitter receiver
