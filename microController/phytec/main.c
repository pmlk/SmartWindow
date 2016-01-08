

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       smartWindow project 
 *
 * @author      Sebastian Kaestner sebastian.kaestner@haw-hamburg.de
 * @author      Sebastian Heinrich sebastian.heinrich@haw-hamburg.de
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

#include "thread.h"
#include "mutex.h"
#include "sendReceive.h"



#define IN

#ifdef IN

//----------------------Servo Anfang

// Header für Servo
#include "periph/pwm.h"
#include "servo.h"

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

static servo_t servo;

#endif

//------------------------Servo Ende


#if ADC_NUMOF < 1
#error "Please enable at least 1 ADC device to run this test"
#endif

#define WAIT             (10000U)
#define RES             ADC_RES_10BIT
#define DELAY           (125U)
#define DELAY2      4U
#define SLEEP       (1000 * 1000U)


#define SW_OPEN_WINDOW              "PUT_Win/Open"
#define SW_CLOSE_WINDOW             "PUT_Win/Close"

#define SW_FINISH_OPEN_WINDOW       SW_OPEN_WINDOW
#define SW_FINISH_CLOSE_WINDOW      SW_CLOSE_WINDOW


#define SENS_TEMP                   "Temp"
#define SENS_AIR_PRESSURE           "AirPressure"
#define SENS_AIR_QUALITY            "AirQuality"
#define SENS_VOLUME                 "Volume"
#define SENS_HUMIDITY               "Humidity"

#define GET_ALL                     "GET_All"
 

static int values[ADC_NUMOF][ADC_MAX_CHANNELS];

struct measuredData_t {
  int16_t temperature;
  uint32_t pressure;
  int volume;
  int humidity;
  int airQuality;
};

#ifdef IN
bool openWindow(servo_t *servo, int *pos, int step);
bool closeWindow(servo_t *servo, int *pos, int step);
#endif

bool initAll(mpl3115a2_t *mplDevice, hdc1000_t *hdcDevice);
struct measuredData_t getMeasuredData(mpl3115a2_t *mplDevice, hdc1000_t *hdcDevice);
int16_t getTemperature(mpl3115a2_t *mplDevice);
uint32_t getPressure(mpl3115a2_t *mplDevice);
int getVolume(void);
int getHumidity(hdc1000_t *hdcDevice);
int getAirQuality(void);
void sendString(char* sendBuffer, char* sensor, char* value);

enum eCommunicationCommand{
    START_MEASUREMENT = 0,
    FINISH_MEASUREMENT,
    #ifdef IN
    START_OPEN_WINDOW,
    START_CLOSE_WINDOW,
    FINISH_OPEN_WINDOW,
    FINISH_CLOSE_WINDOW,
    #endif
};

mutex_t mtx = MUTEX_INIT;

volatile int storage = 1;
kernel_pid_t main_id = KERNEL_PID_UNDEF;
struct measuredData_t data;

char t2_stack[THREAD_STACKSIZE_DEFAULT];

//static char pp_stack[THREAD_STACKSIZE_DEFAULT];
static char pp_buffer[PP_BUF_SIZE];
static msg_t pp_msg_queue[PP_MSG_QUEUE_SIZE];

/*Kommunikations Thread*/
/*empfängt und sendet Nachrichten an den Raspberry PI*/
/*gibt außerdem den Befehl mit Hilfe von einer Messagequeue an den Main-Thread weiter*/
void *communication_thread(void *arg)
{
    (void) arg;
    enum eCommunicationCommand communicationCommand = START_MEASUREMENT;
    msg_t msg;
    char sendBuffer[PP_BUF_SIZE];
    char command[PP_BUF_SIZE];
    char valueString[10];
    //
    //
    //
    //
    struct sockaddr_in6 server_addr;
    char src_addr_str[IPV6_ADDR_MAX_STR_LEN];
    uint16_t port;
    
    msg_init_queue(pp_msg_queue, PP_MSG_QUEUE_SIZE);
    int pp_socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    
    int res;
    struct sockaddr_in6 src;


    port = (uint16_t)PP_PORT;
    if (port == 0) {
        puts("Error: invalid port specified");
        return NULL;
    }
    
    server_addr.sin6_family = AF_INET6;
    memset(&server_addr.sin6_addr, 0, sizeof(server_addr.sin6_addr));
    server_addr.sin6_port = htons(port);
    if (pp_socket < 0) {
        puts("error initializing socket");
        return NULL;
    }
    
    if (bind(pp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        puts("error binding socket");
        return NULL;
    }
    
    // clear buffer
    memset(pp_buffer,0,PP_BUF_SIZE);
    
    printf("Success: started UDP server on port %" PRIu16 "\n", port);
    
    while (1) {
        puts("waitMsg\n");
        
        socklen_t src_len = sizeof(struct sockaddr_in6);
        

        // blocking receive, waiting for data
        if ((res = recvfrom(pp_socket, pp_buffer, sizeof(pp_buffer), 0,
                            (struct sockaddr *)&src, &src_len)) < 0) {
            puts("Error on receive");
        }
        
        else if (res == 0) {
            puts("Peer did shut down");
        }
        
        else { 
           
            inet_ntop(AF_INET6, &(src.sin6_addr),
                      src_addr_str, sizeof(src_addr_str));
            
            strcpy(command, pp_buffer);
            if(strcmp(command, GET_ALL) == 0){
                communicationCommand = START_MEASUREMENT;
                printf("Befehl: %s\n", pp_buffer);
            }
            #ifdef IN
            else if(strcmp(command, SW_OPEN_WINDOW) == 0){
                communicationCommand = START_OPEN_WINDOW;
                printf("Befehl: %s\n", pp_buffer);
            }
            else if(strcmp(command, SW_CLOSE_WINDOW) == 0){
                communicationCommand = START_CLOSE_WINDOW;
                printf("Befehl: %s\n", pp_buffer);
            }
            #endif
            
            
            
            msg.content.value = communicationCommand;
            
            msg_send(&msg, main_id);
            msg_receive(&msg);
            communicationCommand = msg.content.value;

            switch(communicationCommand){
                case START_MEASUREMENT:

                break;
                #ifdef IN
                case START_OPEN_WINDOW: 

                break;
                case START_CLOSE_WINDOW:
                

                break;
                #endif
                case FINISH_MEASUREMENT:
                    //temperature 
                    sprintf(valueString, "%f", (float)(data.temperature) / 10);
                    sendString(sendBuffer, SENS_TEMP, valueString);
                    printf("sendBuffer = %s\n", sendBuffer); 
                    pp_send(src_addr_str, sendBuffer);
                    //pressure
                    sprintf(valueString, "%d", (int)(data.pressure));
                    sendString(sendBuffer, SENS_AIR_PRESSURE, valueString);
                    printf("sendBuffer = %s\n", sendBuffer); 
                    pp_send(src_addr_str, sendBuffer);
                    // volume
                    sprintf(valueString, "%f", (float)(data.volume) / 100);
                    sendString(sendBuffer, SENS_VOLUME, valueString);
                    printf("sendBuffer = %s\n", sendBuffer); 
                    pp_send(src_addr_str, sendBuffer);
                    // humidity
                    sprintf(valueString, "%f", (float)(data.humidity) / 100); 
                    sendString(sendBuffer, SENS_HUMIDITY, valueString);
                    printf("sendBuffer = %s\n", sendBuffer);
                    pp_send(src_addr_str, sendBuffer);
                    // airQuality
                    sprintf(valueString, "%f", (float)(data.airQuality) / 100); 
                    sendString(sendBuffer, SENS_AIR_QUALITY, valueString);
                    printf("sendBuffer = %s\n", sendBuffer);
                    pp_send(src_addr_str, sendBuffer);
                break;
                #ifdef IN
                case FINISH_OPEN_WINDOW:
                    pp_send(src_addr_str, SW_FINISH_OPEN_WINDOW);
                break;

                case FINISH_CLOSE_WINDOW:
                    pp_send(src_addr_str, SW_FINISH_CLOSE_WINDOW);
                break;
                #endif
            }

            
        }
        // clear buffer
        memset(command, 0, res);
        memset(pp_buffer, 0, res);
    }
    return NULL;

}

/*main Thread, kümmert sich um die Initialisierung und steuert den Programmfluß, Sensoren auslesen und Aktoren steuern*/ 
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
    #ifdef IN
    int pos = (STEP_LOWER_BOUND + STEP_UPPER_BOUND) / 2;
    int step = STEP;
    bool b_openWindow = true;
    bool b_windowIsDriving = false;
    #endif
    int communication_thread_id = 0;
    int communicationCommand;

    communication_thread_id = thread_create(
            t2_stack, sizeof(t2_stack),
            THREAD_PRIORITY_MAIN , /*CREATE_WOUT_YIELD |*/ CREATE_STACKTEST,
            communication_thread, NULL, "communication_thread");
    communication_thread_id = communication_thread_id + 1 - 1;
    if (!initAll(&mplDevice, &hdcDevice)) {
        printf("Fehler bei Initialisierung der Devices");
        return 0;
    }
    while (1) {
            
            #ifdef IN 
            if(!b_windowIsDriving){
                //mutex_lock(&mtx);
                msg_receive(&msg);
                communicationCommand = msg.content.value;
                //mutex_unlock(&mtx);
            }
            #else
                //mutex_lock(&mtx);
                msg_receive(&msg);
                communicationCommand = msg.content.value;
                //mutex_unlock(&mtx);    
            #endif
               
        switch(communicationCommand){
            case START_MEASUREMENT:
                //mutex_lock(&mtx);
                data = getMeasuredData(&mplDevice, &hdcDevice);
                //mutex_unlock(&mtx);
                communicationCommand = FINISH_MEASUREMENT; 

                mutex_lock(&mtx);
                msg.content.value = communicationCommand;
                msg_send(&msg, communication_thread_id);
                mutex_unlock(&mtx);   
            break;
            #ifdef IN
            case START_OPEN_WINDOW:
                if(b_openWindow){                    
                    b_windowIsDriving = true;
                    if(openWindow(&servo, &pos, step)){
                        b_openWindow = false;
                        
                        printf("Fenster offen!\n");
                        communicationCommand = FINISH_OPEN_WINDOW;

                        //mutex_lock(&mtx);
                        msg.content.value = communicationCommand;
                        msg_send(&msg, communication_thread_id);
                        //mutex_unlock(&mtx);
                        b_windowIsDriving = false;                        
                    }
                
                }
                else{
                  
                }
       
            break;
            case START_CLOSE_WINDOW:
                if(!b_openWindow) {
                    b_windowIsDriving = true;
                    if(closeWindow(&servo, &pos, step*2)){
                        b_openWindow = true;
                        b_windowIsDriving = false;
                        printf("Fenster geschlossen!\n");
                        communicationCommand = FINISH_CLOSE_WINDOW;

                        mutex_lock(&mtx);
                        msg.content.value = communicationCommand;
                        msg_send(&msg, communication_thread_id);
                        mutex_unlock(&mtx);
                        b_windowIsDriving = false;
                    

                    }
                }
                else{
                }
            break;
            #endif
        }           
        
        xtimer_usleep(WAIT);
    }

    return 0;
}

/*Funktion die den Servo steuert und damit das Fenster schließt*/
/*Gibt zurück, wenn das Fenster fertig geschlossen ist, muss also bis dahin zyklisch aufgerufen werden*/
#ifdef IN
bool closeWindow(servo_t *servo, int *pos, int step){
    servo_set(servo, *pos);
    *pos += step;
    if (*pos >= STEP_UPPER_BOUND) {
        return true;
    } 
    return false;
}
/*Funktion die den Servo steuert und damit das Fenster öffnet*/
/*Gibt zurück, wenn das Fenster fertig geöffnet ist, muss also bis dahin zyklisch aufgerufen werden*/
bool openWindow(servo_t *servo, int *pos, int step){
    servo_set(servo, *pos);
    *pos -= step;
    if(*pos <= STEP_LOWER_BOUND){
        return true;
    }
    return false;
}
#endif

/*Funktion zum Initialisieren aller Aktoren und Sensoren*/
bool initAll(mpl3115a2_t *mplDevice,hdc1000_t *hdcDevice){
    #ifdef IN
    int res;
    #endif
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
    //-------- Beginn Initialisierung Servo --------

    puts("\n");


    #ifdef IN    
    puts("\nRIOT RC servo test");
    puts("Connect an RC servo or scope to PWM_0 channel 0 to see anything");
    res = servo_init(&servo, DEV, CHANNEL, SERVO_MIN, SERVO_MAX);
    if (res < 0) {
        puts("Errors while initializing servo");
        return false;
    }
    puts("Servo initialized.");
    #endif

    //-------- Ende Initialisierung Servo --------

    sw_network_init();

    return true;
}
/*Funktion zum Ermitteln aller Messwerte*/
struct measuredData_t getMeasuredData(mpl3115a2_t *mplDevice, hdc1000_t *hdcDevice){
    struct measuredData_t allMeasuredData;

    allMeasuredData.temperature = getTemperature(mplDevice);
    allMeasuredData.pressure = getPressure(mplDevice);
    allMeasuredData.volume = getVolume();
    allMeasuredData.humidity = getHumidity(hdcDevice);
    allMeasuredData.airQuality = getAirQuality();
    /*Rückgabe in struct, welches alle Messwerte enthält*/

    return allMeasuredData;
}
/*Funktion zur Ermittlung der Temperatur*/
int16_t getTemperature(mpl3115a2_t *mplDevice){
    int16_t temp = 0;

    mpl3115a2_read_temp(mplDevice, &temp);
    /*Die Temperatur wird in °C angegeben und der Wert muss noch durch 10 geteilt werden*/
    /*Ein Wert von 244 entspricht dann als Beispiel 24,4°C*/
    printf("Temperatur: %5i\n", temp);
    return temp;
}
/*Funktion zur Ermittlung des Luftdrucks*/
uint32_t getPressure(mpl3115a2_t *mplDevice){
    uint32_t pressure;
    uint8_t status;

    mpl3115a2_read_pressure(mplDevice, &pressure, &status);
    printf("Druck: %d\n", (int) (pressure));
    return pressure;
}
/*Funktion zum Ermitteln der Lautstärke und Umrechnung in dB*/
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
    /*Lautstärke wird als dB Wert angegebn und der Referenzwert ist 20µPa*/
    printf("Lautstärke in dB: %d\n", volume_temp);
    return volume_temp;
}
/*Funktion zum Ermitteln der Feuchtigkeit*/
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
    /*Ausgabe Feuchtigkeit als relative Feuchtigkeit in Prozent*/
    printf("Luftfeuchtigkeit: %d\n", hum);   
    return hum;
    /* Ende Ausgabe Feuchtigkeitssensor */

}
/*Funktion zum ermitteln der Luftqualität in Prozent*/
int getAirQuality(void){
    int airQuality = 0;
    float airQuality_temp = 0;

    airQuality = adc_sample(0, 2);
    airQuality_temp = (float)(airQuality) /1024 * 100;
    airQuality = (float)(airQuality_temp * 100);
    printf("Luftqualität: %d\n", airQuality);
    return airQuality;
}
/*Funktion zum Erstellen der Befehlsstrings beim Übermitteln der Nachrichten an den Rasperry PI*/
void sendString(char* sendBuffer, char* sensor, char* value){
    
    strcpy(sendBuffer, "PUT_");
    strcat(sendBuffer, sensor);
    /*Abfrage welcher Sensorknoten man selber ist*/
    #ifdef IN
        strcat(sendBuffer, "_IN");
    #else
        strcat(sendBuffer, "_OUT");
    #endif
    strcat(sendBuffer, "/");
    strcat(sendBuffer, value);
}