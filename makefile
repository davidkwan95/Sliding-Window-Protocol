all: 
	make bin/transmitter bin/receiver

bin/transmitter: src/transmitter.cpp src/transFunc.cpp src/transFunc.h src/dcomm.h
	g++ -pthread -o bin/transmitter src/transmitter.cpp src/transFunc.cpp

bin/receiver: src/receiver.cpp src/recvFunc.h src/recvFunc.cpp src/dcomm.h
	g++ -pthread -o bin/receiver src/receiver.cpp src/recvFunc.cpp
