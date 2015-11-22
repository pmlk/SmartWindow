/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test case for the low-level ADC driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifndef TEST_HDC1000_I2C
#error "TEST_HDC1000_I2C not defined"
#endif
#ifndef TEST_HDC1000_ADDR
#error "TEST_HDC1000_ADDR not defined"
#endif

#ifndef TEST_MPL3115A2_I2C
#error "TEST_MPL3115A2_I2C not defined"
#endif
#ifndef TEST_MPL3115A2_ADDR
#error "TEST_MPL3115A2_ADDR not defined"
#endif

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "cpu.h"
#include "board.h"
#include "xtimer.h"
#include "periph/adc.h"
#include "mpl3115a2.h" 
#include "hdc1000.h"
#include "periph/pwm.h"
#include "servo.h"
#include "thread.h"
#include "mutex.h"

 //----------------------Servo Anfang

#define DEV         PWM_0
#define CHANNEL     0

#define SERVO_MIN        (1800U)
#define SERVO_MAX        (2500U)

/* these are defined outside the limits of the servo_init min/max parameters above */
/* we will test the clamping functionality of the servo_set function. */
#define STEP_LOWER_BOUND (1800U)
#define STEP_UPPER_BOUND (2500U)

/* Step size that we move per WAIT us */
#define STEP             (10U)

/* Sleep time between updates, no need to update the servo position more than
 * once per cycle */
#define WAIT             (10000U)

static servo_t servo;

//------------------------Servo Ende

#if ADC_NUMOF < 1
#error "Please enable at least 1 ADC device to run this test"
#endif

#define RES             ADC_RES_10BIT
#define DELAY           (125U)
#define DELAY2      4U
#define SLEEP       (1000 * 1000U)

#define COMMAND_BUFFER 15
static int values[ADC_NUMOF][ADC_MAX_CHANNELS];

struct measuredData_t {
  int16_t temperature;
  uint32_t pressure;
  int volume;
  int humidity;
  int airQuality;
};

bool openWindow(servo_t *servo, int *pos, int step);
bool closeWindow(servo_t *servo, int *pos, int step);

bool initAll(mpl3115a2_t *mplDevice, hdc1000_t *hdcDevice);
struct measuredData_t getMeasuredData(mpl3115a2_t *mplDevice, hdc1000_t *hdcDevice);
int16_t getTemperature(mpl3115a2_t *mplDevice);
uint32_t getPressure(mpl3115a2_t *mplDevice);
int getVolume(void);
int getHumidity(hdc1000_t *hdcDevice);
int getAirQuality(void);

enum eCommunicationCommand{
    START_MEASUREMENT = 0,
    START_OPEN_WINDOW,
    START_CLOSE_WINDOW,
    FINISH_MEASUREMENT,
    FINISH_OPEN_WINDOW,
    FINISH_CLOSE_WINDOW,
};

mutex_t mtx = MUTEX_INIT;

volatile int storage = 1;
kernel_pid_t main_id = KERNEL_PID_UNDEF;
struct measuredData_t data;

char t2_stack[THREAD_STACKSIZE_MAIN];

void *communication_thread(void *arg)
{
    (void) arg;
    enum eCommunicationCommand communicationCommand = START_MEASUREMENT;
    msg_t msg;
    
    char command[COMMAND_BUFFER];
    while(1){
        strcpy(command, "Kommando1");
        
        if(strcmp(command, "Kommando1") == 0){
            communicationCommand = START_MEASUREMENT;
        }
        else if(strcmp(command, "Kommando2") == 0){
            communicationCommand = START_OPEN_WINDOW;
        }
        else if(strcmp(command, "Kommando3") == 0){
            communicationCommand = START_CLOSE_WINDOW;
        }
        else if(strcmp(command, "Kommando4") == 0){
            communicationCommand = FINISH_MEASUREMENT;
        }
        else if(strcmp(command, "Kommando5") == 0){
            communicationCommand = FINISH_OPEN_WINDOW;
        }
        else if(strcmp(command, "Kommando6") == 0){
            communicationCommand = FINISH_CLOSE_WINDOW;
        }
        
        mutex_lock(&mtx);
        msg.content.value = communicationCommand;
        msg_send(&msg, main_id);
        mutex_unlock(&mtx);
        //communicationCommand = START;
        //mutex_lock(&mtx);
        //data = getMeasuredData(&mplDevice, &hdcDevice);
        printf("second thread: %d\n", data.temperature);
        //mutex_unlock(&mtx);
       
        //communicationCommand = FINISH;
        xtimer_sleep(2);
    }
    return NULL;
}


int main(void)
{   
    mpl3115a2_t mplDevice; // device for MPL3115 temperature and pressure sensor
    hdc1000_t hdcDevice; // device for HDC1000 humidity sensor
    /* unused Variables 
    uint32_t pressure;
    int16_t temp;
    uint8_t status;
    
    int hum;
    */
    msg_t msg;
    int pos = (STEP_LOWER_BOUND + STEP_UPPER_BOUND) / 2;
    int step = STEP;
    bool b_openWindow = true;
    int communication_thread_id = 0;
    int communicationCommand;

    communication_thread_id = thread_create(
            t2_stack, sizeof(t2_stack),
            THREAD_PRIORITY_MAIN - 1, CREATE_WOUT_YIELD | CREATE_STACKTEST,
            communication_thread, NULL, "communication_thread");
    communication_thread_id = communication_thread_id + 1 - 1;
    if (!initAll(&mplDevice, &hdcDevice)) {
        printf("Fehler bei Initialisierung der Devices");
        return 0;
    }
    while (1) {
        mutex_lock(&mtx);
        msg_receive(&msg);
        communicationCommand = msg.content.value;
        mutex_unlock(&mtx);

        switch(communicationCommand){
            case START_MEASUREMENT:
                mutex_lock(&mtx);
                data = getMeasuredData(&mplDevice, &hdcDevice);
                mutex_unlock(&mtx);    
            break;
            case START_OPEN_WINDOW:
                if(b_openWindow){
                    if(openWindow(&servo, &pos, step)){
                        b_openWindow = false;
                        mutex_lock(&mtx);
                        // finish in msg
                        mutex_unlock(&mtx);
                    }
                
                }
       
            break;
            case START_CLOSE_WINDOW:
                if(!b_openWindow) {
                    if(closeWindow(&servo, &pos, step*2)){
                        b_openWindow = true;
                        mutex_lock(&mtx);
                        // finish in msg     
                        mutex_unlock(&mtx);
                    }
                }
            break;
        }

        
        
            
        
        xtimer_usleep(WAIT);
    }

    return 0;
}

bool closeWindow(servo_t *servo, int *pos, int step){
    servo_set(servo, *pos);
    *pos += step;
    if (*pos >= STEP_UPPER_BOUND) {
        return true;
    } 
    return false;
}

bool openWindow(servo_t *servo, int *pos, int step){
    servo_set(servo, *pos);
    *pos -= step;
    if(*pos <= STEP_LOWER_BOUND){
        return true;
    }
    return false;
}

bool initAll(mpl3115a2_t *mplDevice,hdc1000_t *hdcDevice){
    int res;
    //-------- Beginn Initialisierung ADC -------- 

    main_id = thread_getpid();

    puts("\nRIOT ADC peripheral driver test\n");
    puts("This test simply converts each available ADC channel about every 10ms\n\n");

    for (int i = 0; i < ADC_NUMOF; i++) {
        /* initialize result vector */
        for (int j = 0; j < ADC_MAX_CHANNELS; j++) {
            values[i][j] = -1;
        }

        /* initialize ADC device */
        printf("Initializing ADC_%i @ %i bit resolution", i, (6 + (2* RES)));
        if (adc_init(i, RES) == 0) {
            puts("    ...[ok]");
        }
        else {
            puts("    ...[failed]");
            return false;
        }
    }

    //-------- Ende Initialisierung ADC --------

    //-------- Beginn Initialisierung Temperatur Sensor --------

    

    puts("MPL3115A2 pressure sensor driver test application\n");
    printf("Initializing MPL3115A2 sensor at I2C_%i... ", TEST_MPL3115A2_I2C);
    if (mpl3115a2_init(mplDevice, TEST_MPL3115A2_I2C, TEST_MPL3115A2_ADDR, MPL3115A2_OS_RATIO_DEFAULT) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return false;
    }
    if (mpl3115a2_set_active(mplDevice)) {
        puts("Measurement start failed.");
        return false;
    }
    //-------- Ende Initialisierung Temperatur Sensor --------
    //-------- Beginn Initialisierung Feuchtigkeits Sensor --------
    
    
    puts("HDC1000 Temperature and Humidity Sensor driver test application\n");
    printf("Initializing HDC1000 sensor at I2C_%i... ", TEST_HDC1000_I2C);
    if (hdc1000_init(hdcDevice, TEST_HDC1000_I2C, TEST_HDC1000_ADDR) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return false;
    }
    

    //-------- Ende Initialisierung Feuchtigkeits Sensor --------

    puts("\n");
    
    puts("\nRIOT RC servo test");
    puts("Connect an RC servo or scope to PWM_0 channel 0 to see anything");
    res = servo_init(&servo, DEV, CHANNEL, SERVO_MIN, SERVO_MAX);
    if (res < 0) {
        puts("Errors while initializing servo");
        return false;
    }
    puts("Servo initialized.");
    return true;
}

struct measuredData_t getMeasuredData(mpl3115a2_t *mplDevice, hdc1000_t *hdcDevice){
    struct measuredData_t allMeasuredData;

    allMeasuredData.temperature = getTemperature(mplDevice);
    allMeasuredData.pressure = getPressure(mplDevice);
    allMeasuredData.volume = getVolume();
    allMeasuredData.humidity = getHumidity(hdcDevice);
    allMeasuredData.airQuality = getAirQuality();


    return allMeasuredData;
}

int16_t getTemperature(mpl3115a2_t *mplDevice){
    int16_t temp = 0;

    mpl3115a2_read_temp(mplDevice, &temp);
    printf("Temperatur: %5i\n", temp);
    return temp;
}

uint32_t getPressure(mpl3115a2_t *mplDevice){
    uint32_t pressure;
    uint8_t status;

    mpl3115a2_read_pressure(mplDevice, &pressure, &status);
    printf("Druck: %d\n", (int) (pressure));
    return pressure;
}

int getVolume(void){
    float volume = 0.0;
    int volume_temp = 0;
    float rms = 0;
    for (int i = 0; i < 1000; i++)
    {
            
        volume_temp = adc_sample(0, 3);
        rms = rms + (volume_temp * volume_temp);
        xtimer_usleep(DELAY);    
    }
    volume = sqrt(rms/1000);
    volume = volume * 1.19976 /10000;
    
    //printf("Lautstärke: %f\n", volume);
    
    volume = 20 * log10(volume/2*100000);
    volume *= 100;
    volume_temp = (int) volume;
    printf("Lautstärke in dB: %d\n", volume_temp);
    return volume_temp;
}

int getHumidity(hdc1000_t *hdcDevice){
    uint16_t rawtemp, rawhum;
    int temp_hum, hum;
    /* Beginn Ausgabe Feuchtigkeitssensor */

    
    if (hdc1000_startmeasure(hdcDevice)) {
        puts("Start measure failed.");
        return -1;
    }
    xtimer_usleep(HDC1000_CONVERSION_TIME);
    hdc1000_read(hdcDevice, &rawtemp, &rawhum);
    //printf("Raw data T: %5i   RH: %5i\n", rawtemp, rawhum);
    hdc1000_convert(rawtemp, rawhum,  &temp_hum, &hum);
    //printf("Data T: %d   RH: %d\n", temp_hum, hum);
    printf("Luftfeuchtigkeit: %d\n", hum);   
    return hum;
    /* Ende Ausgabe Feuchtigkeitssensor */
}

int getAirQuality(void){
    int airQuality = 0;

    airQuality = adc_sample(0, 2);
    printf("Luftqualität: %d\n", airQuality);
    return airQuality;
}
