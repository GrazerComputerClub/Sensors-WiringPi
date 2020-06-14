#include "HC-SR04.h"
#include <wiringPi.h>

volatile BYTE m_TRIGPin, m_ECHOPin;
volatile long long m_StartTime, m_EndTime;
volatile int m_bDataValid;

void ISR_ECHO(void) {
  struct timeval now;

  gettimeofday(&now, 0);
  if (0==m_StartTime) {
    m_StartTime = now.tv_sec*1000000LL + now.tv_usec;
  } else {
    m_EndTime = now.tv_sec*1000000LL + now.tv_usec;
    m_bDataValid = true;
  }
}

HCSR04Sensor::HCSR04Sensor(BYTE TRIGPin, BYTE ECHOPin) {
  m_TRIGPin = TRIGPin;
  m_ECHOPin = ECHOPin;

  wiringPiSetupGpio();
  pinMode(m_TRIGPin, OUTPUT);
  wiringPiISR(m_ECHOPin, INT_EDGE_BOTH, &ISR_ECHO);
}

HCSR04Sensor::~HCSR04Sensor() {
  pinMode(m_ECHOPin, INPUT);
  digitalWrite(m_TRIGPin, LOW);
  pinMode(m_TRIGPin, INPUT);
}

void HCSR04Sensor::StartDistanceMeas() {
  m_bDataValid = false;
  m_fDistance = 0.0;
  m_StartTime = 0;
  digitalWrite(m_TRIGPin, HIGH); 
  delayMicroseconds(20000);
  gettimeofday(&m_MeasStartTime, 0);
  digitalWrite(m_TRIGPin, LOW); //start impulse
  //delayMicroseconds(80000);
}

bool HCSR04Sensor::DataValid() {
  if (m_bDataValid && m_EndTime>m_StartTime) {
    m_fDistanceTime = (m_EndTime - m_StartTime) / 1000000.0f;
    m_fDistance = m_fSpeedOfSound * 100.0f * m_fDistanceTime / 2.0f;//cm
    return true;
  } else {
    //keep waiting
  };
  return false;
}

float HCSR04Sensor::Distance() {
  return m_bDataValid ? m_fDistance : 0.0f;
};

float HCSR04Sensor::DistanceTime() {
  return m_bDataValid ? m_fDistanceTime : 0.0f;
};




