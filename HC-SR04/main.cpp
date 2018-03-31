// Example HC-SR04 Sensor (ultrasonic distance)
// Code Licence: CC BY 4.0 (https://creativecommons.org/licenses/by/4.0/)
// written by m.stroh

#include "HC-SR04.h"
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

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
  HCSR04Sensor HCSR04(nTRIGPin, nECHOPin);

  while(!end) {
    HCSR04.StartDistanceMeas();
    usleep(80000); //maxium distance 80ms=
    if (HCSR04.DataValid()) {
      printf("distance: %.1f cm (time elapsed: %.2f ms)\n",
      HCSR04.Distance(), HCSR04.DistanceTime()*1000.0f);
      usleep(30000);
    }
  }
  return 0;
}
