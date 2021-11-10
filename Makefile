# -*- Makefile -*-
all:clean emitter receiver

emitter:
	gcc -o "emitter" ./data_link/packet.c ./data_link/dl.c ./data_link/state.c emitter.c

receiver:
	gcc -o "receiver" ./data_link/packet.c ./data_link/dl.c ./data_link/state.c receiver.c

clean:
	rm -f emitter receiver
