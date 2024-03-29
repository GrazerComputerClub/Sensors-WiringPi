// Example HC-SR04 Sensor (ultrasonic distance)
// Code Licence: CC BY 4.0 (https://creativecommons.org/licenses/by/4.0/)
// written by m.stroh
// Compile: g++ -o HC-SR04 HC-SR04.cpp main.cpp -lwiringPi

#include "HC-SR04.h"
#include <stdio.h>
#include <signal.h>
#include <string.h>

static sig_atomic_t end = 0;


static void sighandler(int signo) {
  end = 1;
}

int main(void) {
  struct sigaction sa;

  memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = sighandler;
  sigaction(SIGINT,  &sa, NULL);
  sigaction(SIGQUIT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);

  int nTRIGPin = 17;
  int nECHOPin = 27;
  printf("HC-SR04 distance sensor (Trig: %d, Echo: %d)\n", nTRIGPin, nECHOPin);
  HCSR04Sensor HCSR04(nTRIGPin, nECHOPin);

  while(!end) {
    HCSR04.StartDistanceMeas();
    usleep(80000); //maxium distance 80ms=
    if (HCSR04.DataValid()) {
      printf("distance: %5.1f cm (time elapsed: %.2f ms)\n",
      HCSR04.Distance(), HCSR04.DistanceTime()*1000.0f);
      usleep(30000);
    } else {
      printf("timeout\n");
      sleep(2);
    }
    fflush(stdout);
  }
  printf("\n");
  return 0;
}

