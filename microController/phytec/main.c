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

#include "cpu.h"
#include "board.h"
#include "xtimer.h"
#include "periph/adc.h"
#include "mpl3115a2.h" 
#include "hdc1000.h"
#include "periph/pwm.h"
#include "servo.h"


 //----------------------Servo Anfang

 #define DEV         PWM_0
#define CHANNEL     0

#define SERVO_MIN        (1000U)
#define SERVO_MAX        (2000U)

/* these are defined outside the limits of the servo_init min/max parameters above */
/* we will test the clamping functionality of the servo_set function. */
#define STEP_LOWER_BOUND (900U)
#define STEP_UPPER_BOUND (2100U)

/* Step size that we move per WAIT us */
#define STEP             (10U)

/* Sleep time between updates, no need to update the servo position more than
 * once per cycle */
#define WAIT             (10000U)

//static servo_t servo;

//------------------------Servo Ende

#if ADC_NUMOF < 1
#error "Please enable at least 1 ADC device to run this test"
#endif

#define RES             ADC_RES_10BIT
#define DELAY           (100U)
#define DELAY2      2
#define SLEEP       (1000 * 1000U)

static int values[ADC_NUMOF][ADC_MAX_CHANNELS];

int main(void)
{

    //-------- Beginn Initialisierung ADC -------- 

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
            return 1;
        }
    }

    //-------- Ende Initialisierung ADC --------

    //-------- Beginn Initialisierung Temperatur Sensor --------

   /* mpl3115a2_t dev;
    uint32_t pressure;
    int16_t temp;
    uint8_t status;

    puts("MPL3115A2 pressure sensor driver test application\n");
    printf("Initializing MPL3115A2 sensor at I2C_%i... ", TEST_MPL3115A2_I2C);
    if (mpl3115a2_init(&dev, TEST_MPL3115A2_I2C, TEST_MPL3115A2_ADDR,
                                                 MPL3115A2_OS_RATIO_DEFAULT) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    if (mpl3115a2_set_active(&dev)) {
        puts("Measurement start failed.");
        return -1;
    }

    //-------- Ende Initialisierung Temperatur Sensor --------

    //-------- Beginn Initialisierung Feuchtigkeits Sensor --------

    hdc1000_t device;
    uint16_t rawtemp, rawhum;
    int temp_hum, hum;

    puts("HDC1000 Temperature and Humidity Sensor driver test application\n");
    printf("Initializing HDC1000 sensor at I2C_%i... ", TEST_HDC1000_I2C);
    if (hdc1000_init(&device, TEST_HDC1000_I2C, TEST_HDC1000_ADDR) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    */

    //-------- Ende Initialisierung Feuchtigkeits Sensor --------

    /* puts("\n");

    int res;
    int pos = (STEP_LOWER_BOUND + STEP_UPPER_BOUND) / 2;
    int step = STEP;

    puts("\nRIOT RC servo test");
    puts("Connect an RC servo or scope to PWM_0 channel 0 to see anything");

    res = servo_init(&servo, DEV, CHANNEL, SERVO_MIN, SERVO_MAX);
    if (res < 0) {
        puts("Errors while initializing servo");
        return -1;
    }
    puts("Servo initialized.");

    */

    while (1) {

        /* Beginn Ausgabe ADC */
        /* convert each channel for this ADC device */
        /* for (int i = 0; i < ADC_NUMOF; i++) {
            for (int j = 0; j < ADC_MAX_CHANNELS; j++) {
                values[i][j] = adc_sample(i, j);
            }
        }
        */

        /* print the results */
        /*
        printf("Values: ");
        for (int i = 0; i < ADC_NUMOF; i++) {
            for (int j = 0; j < ADC_MAX_CHANNELS; j++) {
                if (values[i][j] >= 0) {
                    printf("ADC_%i-CH%i: %4i  ", i, j, values[i][j]);
                }
            }
        }
        printf("\n");
        */

        /* Ende Ausgabe ADC */

        /* Beginn Ausgabe Temperatursensor */

        /*
        xtimer_usleep(SLEEP);
        mpl3115a2_read_pressure(&dev, &pressure, &status);
        printf("Pressure: %u Status: %#02x\n", (unsigned int)pressure, status);
        mpl3115a2_read_temp(&dev, &temp);
        printf("Temperature: %d\n", temp/10);
        */

        /* Ende Ausgabe Temperatursensor */

        /* Beginn Ausgabe Feuchtigkeitssensor */

        /*
        if (hdc1000_startmeasure(&device)) {
            puts("Start measure failed.");
            return -1;
        }
        xtimer_usleep(HDC1000_CONVERSION_TIME);

        hdc1000_read(&device, &rawtemp, &rawhum);
        printf("Raw data T: %5i   RH: %5i\n", rawtemp, rawhum);

        hdc1000_convert(rawtemp, rawhum,  &temp_hum, &hum);
        printf("Data T: %d   RH: %d\n", temp_hum, hum);
        */

        /* Ende Ausgabe Feuchtigkeitssensor */
           
        float max = 0.0;
        int volume = 0;

        for (int i = 0; i < 100; i++)
        {
            
            volume = adc_sample(0, 3);

            if (((float) (volume)) > max)
            {
                max = (float) (volume) * 0.277 + 40.0;
            }

            xtimer_usleep(DELAY);
            //printf("%d\n", max);
        }

        printf("Maximum: %f\n", max);
        xtimer_sleep(DELAY2);        

        //xtimer_sleep(DELAY2);


        /* sleep a little while */
        //xtimer_usleep(DELAY);
        //xtimer_sleep(DELAY2);

        /* servo_set(&servo, pos);

        pos += step;
        if (pos <= STEP_LOWER_BOUND || pos >= STEP_UPPER_BOUND) {
            step = -step;
        }

        xtimer_usleep(WAIT);

        */

    }

    return 0;
}
