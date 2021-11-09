# -*- Makefile -*-
all:clean emitter receiver

emitter:
	gcc -o "emitter" ./data_link/packet.c ./data_link/dl.c emitter.c

receiver:
	gcc -o "receiver" ./data_link/packet.c ./data_link/dl.c receiver.c

clean:
	rm -f emitter receiver
