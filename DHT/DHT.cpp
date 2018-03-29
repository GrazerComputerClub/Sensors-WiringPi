// Functions for DHT11/22 or AM2302 Sensor (temperature & humidity)
// Code Licence: CC BY 4.0 (https://creativecommons.org/licenses/by/4.0/)
// written by m.stroh

#include "DHT.h"
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


void DHTSensor::ZeroData() {
  m_DHTData[0] = m_DHTData[1] = 0;
  m_DHTData[2] = m_DHTData[3] = 0;
  m_DHTData[4] = 0;
  m_bDataValid = true;
  m_fTemp = 0.0f;
  m_fRH = 0.0f;
}

DHTSensor::DHTSensor(BYTE DATAPin, BYTE Retries) {
  m_DATAPin = DATAPin;
  m_Retries = Retries;
  if (Retries<1) {
    m_Retries = 1;
  }
  if (Retries>60) {
    m_Retries = 60;
  }
  m_bDebug = false;
  ZeroData();
  m_Tries = 0;
  wiringPiSetupGpio();
}

DHTSensor::~DHTSensor() {
}

long long DHTSensor::GetTimeuS() {
  struct timeval te;
  gettimeofday(&te, NULL); // get current time
  long long milliseconds = te.tv_sec*1000000LL + te.tv_usec;
  return milliseconds;
}

int DHTSensor::GetDHTData(int StartPosition) {
  ZeroData();
  int nBitNo = 0;
  for (int i=StartPosition; i<StartPosition+40; i++) {
    int nByteNo = nBitNo/8;
    m_DHTData[nByteNo] <<= 0x01; //move bit
    if (m_bDebug) printf("counter[%2d] = %lld us ", i, m_TimeOfChange[i]-m_TimeOfChange[i-1]);

    if (m_TimeOfChange[i]-m_TimeOfChange[i-1] > cnThresholdHighLow) {
      m_DHTData[nByteNo] |= 0x01;   //set bit
      if (m_bDebug) printf(" -> 1 (Bit: %02d)\n", nBitNo);
    } else {
      if (m_bDebug) printf(" -> 0 (Bit: %02d)\n", nBitNo);
    }
    nBitNo++;
  }

  if (m_DHTData[4] == ((m_DHTData[0] + m_DHTData[1] + m_DHTData[2] + m_DHTData[3]) & 0xFF)) {
    //if (m_bDebug) printf("CRC OK\n");
    if (m_DHTData[0] <=3) { // Autodedect DHT type
      // Calculation DHT22/AM2302
      m_fRH = (float)m_DHTData[0] * 256 + (float)m_DHTData[1];
      m_fRH /= 10.0f;
      m_fTemp = (float)(m_DHTData[2] & 0x7F) * 256 + (float)m_DHTData[3];
      m_fTemp /= 10.0f;
      if (m_DHTData[2] & 0x80) {
        m_fTemp *= -1;
      }
    } else {
      // Calculation DHT11
      m_fRH = m_DHTData[0] * 10 + m_DHTData[1];
      m_fRH /= 10.0f;
      m_fTemp = m_DHTData[2] * 10 + m_DHTData[3];
      m_fTemp /= 10.0f;
    }
    m_bDataValid = true;
    if (m_bDebug) printf("Humidity = %.2f %% Temperature = %.2f *C\n", m_fRH, m_fTemp);
    return EXIT_SUCCESS;
  } else {
    if (m_bDebug) printf("CRC ERROR\n");
    return EXIT_FAILURE;
  }
}


int DHTSensor::ReadDHT() {
  int laststate, state;
  int ReadCounter;

  for (m_Tries=1; m_Tries<m_Retries+1; m_Tries++) {
    //Start sequence, Low , High
    pinMode(m_DATAPin, OUTPUT);
    digitalWrite(m_DATAPin, LOW);
    delayMicroseconds(20000); //DHT11 needs >18ms
    digitalWrite(m_DATAPin, HIGH);
    delayMicroseconds(30);
    laststate = HIGH;
    pinMode(m_DATAPin, INPUT);
    // Chip start to transfer
    state = digitalRead(m_DATAPin);
    for (int i=0; i<cnMaxTransferReads; i++) {
      ReadCounter = 0;
      do {
        ReadCounter++;
        laststate = state;
        delayMicroseconds(1);
        state = digitalRead(m_DATAPin);
      } while (!(laststate==HIGH && state==LOW) && ReadCounter<100); //High->Low trigger
      laststate = LOW;
      m_TimeOfChange[i] = GetTimeuS();
    }
    if (EXIT_SUCCESS==GetDHTData(1)) {
      break;
    } else {
      if (m_bDebug) printf("400 ms break\n");
      delayMicroseconds(400000);
    }
  };
  return m_bDataValid;
}
