// Example for DHT library
// Code Licence: CC BY 4.0 (https://creativecommons.org/licenses/by/4.0/)
// written by m.stroh

#include "DHT.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
  int nDATAPin = 17;

  if (argc < 2) {
    printf("usage: %s <Pin>\n", argv[0]);
    printf("  <Pin> is the BCM pin number (GPIO 4 default)\n");
  } else {
    nDATAPin = atoi(argv[1]);
	}
  printf("Raspberry Pi wiringPi DHT11/22 (AM2303) reader\n") ;
	DHTSensor DHT(nDATAPin, 3);
	DHT.ReadDHT();
	if (DHT.DataValid()) {
		printf("Humidity = %.2f %% Temperature = %.2f *C\n", DHT.RH(), DHT.Temp());		
	} else {
		printf("Error, DHT sensor not responding"); 
	}
 
  return 0 ;
}
