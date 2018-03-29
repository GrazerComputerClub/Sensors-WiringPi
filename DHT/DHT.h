// Functions for DHT11/22 or AM2302 Sensor (temperature & humidity)
// Code Licence: CC BY 4.0 (https://creativecommons.org/licenses/by/4.0/)
// written by m.stroh

#ifndef __DHTSensor__
#define __DHTSensor__


// data types
typedef unsigned char BYTE;


class DHTSensor {

  public:
    DHTSensor(BYTE DATAPin, BYTE Retries);
    ~DHTSensor();
    int   ReadDHT();
    float Temp() { return m_bDataValid ? m_fTemp : 0.0f; };
    float RH()  { return  m_bDataValid ? m_fRH   : 0.0f; };
    int   Tries()  { return  m_Tries; };
    bool  DataValid() { return m_bDataValid; };

  private:
    static const int cnMaxTransferReads = 41;
    // low=50us+25us=75us, high=50us+70us=120us
    const int cnThresholdHighLow = 100; //us

    BYTE m_DATAPin;
    BYTE m_Retries;
    BYTE m_Tries;
    int  m_DHTData[5];
    long long m_TimeOfChange[cnMaxTransferReads];
    bool m_bDebug;
    bool m_bDataValid;
    float m_fTemp, m_fRH;

  private:
    long long GetTimeuS();
    void ZeroData();
    int GetDHTData(int StartPosition);
};


#endif //__DHTSensor__
