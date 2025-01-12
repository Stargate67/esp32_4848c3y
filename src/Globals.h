
#include <Arduino.h>
#include <ModbusIP_ESP8266.h>
#include "esp32_smartdisplay.h"

#define SERDEBUG true

extern ModbusIP mb;  //ModbusIP object

extern String sClockHHMMSS;
extern String sClockHHMM;
extern String sPrintdate;
extern String sPrintShortdate;

extern String sExtMaxTimeStp;
extern String sExtMinTimeStp;

extern const int iAvgMaxFifo;
extern float fAvgFiFo[];
extern int iReadIndex;
extern int iStartIndex;

extern const IPAddress MBremote;   // Address of Internet Box 
extern const int START_REG;       // Starting holding register
extern const int NUM_REGS;           // Number of holding registers to read
extern const int MB_READ_INTERVAL;         // Interval between reads (in milliseconds)

extern lv_obj_t *lblScrolTxt_1;
extern lv_obj_t *lblScrolTxt_2;
extern lv_obj_t *lblScrolTxt_3;

extern lv_obj_t *IPLabel;
extern lv_obj_t *AlarmLabel;
extern lv_obj_t *ClockLabel;
extern lv_obj_t *HorScrollArea;

/*
lv_obj_t *scr;
*/

