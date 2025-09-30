#ifndef APP_H_
#define APP_H_

#include "hal_data.h"

/* Pin assignment */
#define RELAY_PIN   BSP_IO_PORT_05_PIN_03   // Example GPIO pin for relay control

/* Task Prototypes */
void SensorTask(INT stacd, void *exinf);
void PumpTask(INT stacd, void *exinf);

#endif /* APP_H_ */
