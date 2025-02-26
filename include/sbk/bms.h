#ifndef __SBK_BMS_H
#define __SBK_BMS_H


#include <sbk/macro.h>

#include <stdint.h>


#define SBK_BMS_OFF

typedef struct __PACKED {
	uint8_t  off;  // set to SBK_BMS_OFF to turn off the battery
	uint8_t  _r[3];
} SbkBmsCtrl;


#define SBK_BMS_WAKEUP          0x00
#define SBK_BMS_DISCHARGE       0x01
#define SBK_BMS_CHARGE          0x02
#define SBK_BMS_CHARGER         0x03
#define SBK_BMS_PRECHARGE       0x04
#define SBK_BMS_CHARGE_ERROR    0x05
#define SBK_BMS_WATERFALL_LIGHT 0x06
#define SBK_BMS_SELF_DISCHARGE  0x07
#define SBK_BMS_JUNK            0x08

typedef struct __PACKED {
	uint8_t  version[2];
	uint8_t  status;
	uint8_t  soc;
	uint32_t current;     // [mA]
	uint16_t cycle;
	int16_t   bqNtc[2];    // [Co]
	int16_t   mcuNtc[2];   // [Co]
	uint16_t voltage[8]; // [mV]
} SbkBmsHighFb;

typedef struct __PACKED {
	uint8_t  version[2];
	uint8_t  status;
	uint8_t  soc;
	uint32_t current;     // [mA]
	uint16_t cycle;
	int8_t   bqNtc[2];    // [Co]
	int8_t   mcuNtc[2];   // [Co]
	uint8_t voltage[10]; // [mV]
} SbkBmsFb;


#endif // !__SBK_BMS_H
