CC = g++
CFLAGS = -Wall 

all: HCSR04

HCSR04: 
	${CC} ${CFLAGS} HC-SR04.cpp main.cpp -o HC-SR04 -Wall -lwiringPi

clean:
	rm -f HC-SR04
