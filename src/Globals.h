
#include <Arduino.h>
#include <ModbusIP_ESP8266.h>

extern ModbusIP mb;  //ModbusIP object

extern String sClockHHMMSS;
extern String sClockHHMM;
extern String sPrintdate;
extern String sPrintShortdate;

extern String sExtMaxTimeStp;
extern String sExtMinTimeStp;


extern const int numReadings = 10;
extern float readings[numReadings];
extern int iReadIndex;
extern int iStartIndex;


//IPAddress MBremote(192, 168, 0, 105);  // Address of Modbus Slave device
extern IPAddress MBremote(77, 204, 15, 6);   // Address of Internet Box 
extern const int START_REG = 12688;       // Starting holding register
extern const int NUM_REGS = 10;           // Number of holding registers to read
extern const int MB_READ_INTERVAL = 1000;         // Interval between reads (in milliseconds)
