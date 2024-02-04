#include "HC-SR04.h"
#include <wiringPi.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

volatile BYTE m_TRIGPin, m_ECHOPin;
volatile bool m_Sysfs;
volatile bool m_Debug;
volatile long long m_StartTime, m_EndTime;
volatile int m_bDataValid;


void execute(const char* fmt, ...) {
  const int maxlength=1024;
  char cmd[maxlength+1];
  va_list args;

  va_start(args, fmt);
  vsnprintf(cmd, maxlength, fmt, args);
  va_end(args);
  if (m_Debug) printf("execute: \"%s\"", cmd);
  int ret = system(cmd);
  if (m_Debug) printf(" returned %d\n", ret);
}

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

HCSR04Sensor::HCSR04Sensor(BYTE TRIGPin, BYTE ECHOPin, bool Sysfs) {
  m_TRIGPin = TRIGPin;
  m_ECHOPin = ECHOPin;
  m_Sysfs = Sysfs;
  m_Debug = false;

  if (getenv ("HCSR04_DEBUG") != NULL) {
    printf ("HCSR04: debug mode enabled\n") ;
    m_Debug = TRUE ;
  }

  if (m_Sysfs) {
    // need to export before wiringPiSetupSys !
    execute("gpio export %d out", m_TRIGPin);
    execute("gpio export %d in", m_ECHOPin);
    if (m_Debug) execute("gpio exports");
    wiringPiSetupSys();
    digitalWrite(m_TRIGPin, HIGH);
    sleep(1);
    digitalWrite(m_TRIGPin, LOW);
  } else {
    wiringPiSetupGpio();
    pinMode(m_TRIGPin, OUTPUT);
  }
  wiringPiISR(m_ECHOPin, INT_EDGE_BOTH, &ISR_ECHO);
}

HCSR04Sensor::~HCSR04Sensor() {
  if (m_Sysfs) {
    execute("gpio export %d in", m_TRIGPin);
    execute("gpio unexport %d", m_TRIGPin);
    execute("gpio unexport %d", m_ECHOPin);
    if (m_Debug) execute("gpio exports");
  } else {
    pinMode(m_ECHOPin, INPUT);
    digitalWrite(m_TRIGPin, LOW);
    pinMode(m_TRIGPin, INPUT);
  }
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

