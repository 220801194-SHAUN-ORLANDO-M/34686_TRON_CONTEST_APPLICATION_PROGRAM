#include "app.h"
#include "kernel.h"
#include "hal_data.h"

#define MOISTURE_THRESHOLD 1800   // ADC threshold value
#define TASK_STACK_SIZE    1024

/* RTOS Tasks */
void SensorTask(INT stacd, void *exinf);
void PumpTask(INT stacd, void *exinf);

/* Task IDs */
ID tskid_sensor;
ID tskid_pump;

/* Shared variable */
volatile uint16_t soil_moisture = 0;
volatile uint8_t pump_state = 0;

void main(void) {
    ER ercd;

    /* Initialize hardware (ADC + GPIO for relay) */
    R_IOPORT_Open(&g_ioport_ctrl, g_ioport.p_cfg);
    R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);

    /* Create SensorTask */
    T_CTSK sensor_task = {
        .tskatr = TA_HLNG | TA_RNG3,
        .task = (FP)SensorTask,
        .itskpri = 5,
        .stksz = TASK_STACK_SIZE
    };
    ercd = tk_cre_tsk(&sensor_task);
    tskid_sensor = ercd;

    /* Create PumpTask */
    T_CTSK pump_task = {
        .tskatr = TA_HLNG | TA_RNG3,
        .task = (FP)PumpTask,
        .itskpri = 6,
        .stksz = TASK_STACK_SIZE
    };
    ercd = tk_cre_tsk(&pump_task);
    tskid_pump = ercd;

    /* Start tasks */
    tk_sta_tsk(tskid_sensor, 0);
    tk_sta_tsk(tskid_pump, 0);

    /* Kernel main loop */
    tk_slp_tsk(TMO_FEVR);
}

/* Reads soil moisture value every 200ms */
void SensorTask(INT stacd, void *exinf) {
    adc_status_t status;
    while(1) {
        R_ADC_ScanStart(&g_adc0_ctrl);
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
        R_ADC_StatusGet(&g_adc0_ctrl, &status);
        soil_moisture = status.data;  // Replace with actual ADC read function
        tk_dly_tsk(200); // 200ms
    }
}

/* Controls pump ON/OFF */
void PumpTask(INT stacd, void *exinf) {
    while(1) {
        if (soil_moisture < MOISTURE_THRESHOLD && !pump_state) {
            R_IOPORT_PinWrite(&g_ioport_ctrl, RELAY_PIN, BSP_IO_LEVEL_HIGH);
            pump_state = 1;
        } else if (soil_moisture >= MOISTURE_THRESHOLD && pump_state) {
            R_IOPORT_PinWrite(&g_ioport_ctrl, RELAY_PIN, BSP_IO_LEVEL_LOW);
            pump_state = 0;
        }
        tk_dly_tsk(500); // 0.5s control loop
    }
}
