// Functions for HC-SR04 Sensor (ultrasonic distance)
// Code Licence: CC BY 4.0 (https://creativecommons.org/licenses/by/4.0/)
// written by m.stroh

#ifndef __HC_SR04__
#define __HC_SR04__

#include <sys/time.h>

// data types
typedef unsigned char BYTE;

class HCSR04Sensor {

  public:
    HCSR04Sensor(BYTE TRIGPin, BYTE ECHOPin);
    ~HCSR04Sensor();
    void  StartDistanceMeas();
    float Distance(); //cm
    float DistanceTime(); 
    bool  DataValid();
    void  SetSpeedOfSound(double fSpeedOfSound) { // m/s
      m_fSpeedOfSound = fSpeedOfSound;
    }

  private:
    float m_fSpeedOfSound = 343.42; // m/s, at 20*C
    float m_fDistance, m_fDistanceTime;
    struct timeval m_MeasStartTime;
};


#endif //__HC_SR04__
