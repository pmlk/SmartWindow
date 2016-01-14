/**
 * @file
 * <h1>Smart University</h1>
 * <h2>Smart Window</h2>
 *
 * tested on RaspberryPi+<br/>
 * This program<br/>
 * <ul>
 * 	<li>requests data from RIOT sensor nodes</li>
 * 	<li>saves sensor data in mysql database</li>
 * 	<li>makes decision based on sensor data to open or close (a) window(s)</li>
 * </ul>
 *
 */
/*
 * Authors: pmlk, shein318
 */

// standard
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// network
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <net/if.h>
// threading
#include <pthread.h>

// only for dummy
#include <time.h>

#include <string.h> /* for strncpy */
#include <sys/types.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>

//
#include "SmartWindowMacros.h"
#include "write_read.h"

#ifdef XCOMPILE		// defined in IDE (eclipse)
// when Cross-Compiling
#include "xcompile/headers/mysql.h"
#else
// when compiling locally on RasPi
#include <mysql/mysql.h>
#endif

// from RIOT:
// http://riot-os.org/api/group__net__ipv6__addr.html#ga2713917d7f8462406ab96eb14f1bfa2c
#define IPV6_ADDR_MAX_STR_LEN   (sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"))
#define MAXBUF					(2048)

// polling interval to ask sensor nodes for data
// update rate in seconds
#define UPDATE_RATE				10

// MYSQL
#define DB_NAME					"SmartWindow"
// Table Names
#define TBL_AUTO						"AutoModus"
#define TBL_MAN_OPEN					"ManOpen"
#define TBL_MAN_CLOSE					"ManClose"
#define TBL_WIN_STATUS					"Win_Open"
// Table suffixes
#define TBL_SUF_ALARMSTATE				"AlarmState"
// threshold table name suffixes
#define THR_MIN					"MIN"
#define	THR_MAX					"MAX"

// create table name with sensor name and suffix
// i.e.: TEMP_IN
#define CONCAT_TABLE(sensor,suffix)		tableName(sensor,suffix)

// statuses
#define STS_OK			"0"
#define STS_WARNING 	"1"
#define STS_ALARM		"2"

//
// local function declarations
//
// thread functions (loops)
void *receiveLoop(void* args);
void *decisionLoop(void* args);
void *dummyLoop(void* args);
// helper functions
int sw_send(char *dst_addr, char *data);
int get_link_local_addr(char* if_name, int if_name_length, struct sockaddr_in6 *ip);
int get_Lowpan0Ipv6Address(char* ipv6_strBuf, size_t bufferLen);
int splitString(char* string, char* delimiter, char ***resultArr);
char* concat(char *s1, char *s2);
char* tableName(char* sensor, char* suffix);
void writeAirQState(char* state);
void writeHumdityState(char* state);
void writeVolumeState(char* state);
void writeTemperatureState(char* state);

/**
 * starts threads and
 * periodically sends multicast requests
 * for sensor data
 */
int main(int argc, char **argv)
{
	// declare and start threads
	pthread_t rcvThread, decisionThread;//, dummyDataThread;
	pthread_create(&rcvThread, NULL, &receiveLoop, (void*)NULL);
	pthread_create(&decisionThread, NULL, &decisionLoop, (void*)NULL);
	sleep(1);

	// requesting sensor data periodically
	while(1)
	{
		// request data
		sw_send(DST_MULITCAST, GET_ALL);
		sleep(UPDATE_RATE);
	}

	return 0;
}

/**
 * sends udp message to \p dst_addr
 * @param dst_addr address to send \p data to
 * @param data payload data to send
 * @return
 */
int sw_send(char *dst_addr, char *data)
{
	struct sockaddr_in6 src, dst;
	size_t data_len = strlen(data);
	int port = PORT;
	int sock;
	src.sin6_family = AF_INET6;
	dst.sin6_family = AF_INET6;
	memset(&src.sin6_addr, 0, sizeof(src.sin6_addr));

	if (inet_pton(AF_INET6, dst_addr, &dst.sin6_addr) != 1) {
		printf("Error: unable to parse destination address: %s", dst_addr);
		return 1;
	}

	dst.sin6_port = htons(port);
	dst.sin6_scope_id = 5;
	src.sin6_port = htons(port);
	sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		puts("error initializing socket");
		return 1;
	}

	int bytesSent = 0;
	bytesSent = sendto(sock, data, data_len, 0, (struct sockaddr *)&dst, sizeof(dst));
	//printf("sent\t\"%s\"\tto\t%s\t(%i Bytes)\n",data, dst_addr, bytesSent);

	close(sock);
	return bytesSent;
}

/**
 * thread listening for messages<br/>
 * writes sensor data to database
 * @param args not used
 */
void *receiveLoop(void* args)
{
	int sock;
	int status;
	char src_addr_str[IPV6_ADDR_MAX_STR_LEN];
	char srv_addr_str[IPV6_ADDR_MAX_STR_LEN];
	struct sockaddr_in6 sin6;
	unsigned int sin6len;
	char buffer[MAXBUF];

	sock = socket(PF_INET6, SOCK_DGRAM,0); // create udp socket
	sin6len = sizeof(struct sockaddr_in6);

	memset(&sin6, 0, sin6len);
	sin6.sin6_port = htons(PORT);	// set port
	sin6.sin6_family = AF_INET6;	// set address family, use ipv6
	sin6.sin6_addr = in6addr_any;	// receive on any address


	// bind socket
	status = bind(sock, (struct sockaddr *)&sin6, sin6len);
	if(-1 == status)
		perror("recvLoop bind"), exit(1);

	status = getsockname(sock, (struct sockaddr *)&sin6, &sin6len);
	get_Lowpan0Ipv6Address(srv_addr_str, IPV6_ADDR_MAX_STR_LEN);

	//printf("Lowpan0 ipv6: %s\n", srv_addr_str);
	printf("started UDP server, listening on port %i\n", PORT);

	MYSQL  *mysql = mysql_init(NULL);						// initialize mysql handle
	//struct CGI_DATEN *messdaten = new CGI_DATEN();			//

	while(status >= 0)
	{
		struct sockaddr_in6 src;
		socklen_t src_len = sizeof(struct sockaddr_in6);

		// clear buffer memory
		memset(buffer,0, MAXBUF);

		// blocking receive, waiting for data
		if ((status = recvfrom(sock, buffer, sizeof(buffer), 0,
				(struct sockaddr *)&src, &src_len)) < 0) {
			puts("Error on receive");
		}
		else if (status == 0) {
			puts("Peer did shut down");
		}
		else
		{
			// successfully received message

			// get sender ip address
			inet_ntop(AF_INET6, &(src.sin6_addr), src_addr_str, sizeof(src_addr_str));

			// print raw message from sender
			printf("rcvd\t\"%s\"\tfrom\t%s\t(%i Bytes)\n", buffer, src_addr_str, status);

			//if(true)
			// ignore messages from self, comment out (use if(true)) when using dummy loop!
			if(strcmp(srv_addr_str, src_addr_str) != 0)
			{
				// array will contain: [CMD_SENSOR_POSITION][VALUE], i.e. [PUT_TEMP_IN][22.12]
				char **sensornameAndValue = NULL;
				int numStr = splitString(buffer,SEP_VALUE, &sensornameAndValue);

				// expecting CMD_SENSOR_POS/VALUE, split by '/' => 2 strings
				if(numStr == 2)
				{
					// array will contain: [CMD][SENSOR][POSITION], i.e. [PUT][TEMP][IN]
					char **cmdSensorPos = NULL;
					int numSubStr = splitString(sensornameAndValue[0],SEP_SENSOR, &cmdSensorPos);

					// expecting CMD_SENSOR_POS, split by '_' => 3 strings
					if(numSubStr == 3)
					{
						// assign relevant variables
						char *cmd = cmdSensorPos[0];
						char *sensor = cmdSensorPos[1];
						char *pos = cmdSensorPos[2];
						char *value = sensornameAndValue[1];
						char *table = tableName(sensor,pos);

						// printf("cmd: %s\nsensor: %s\npos:%s\nvalue: %s\n", cmd, sensor, pos, value);
						// printf("table: %s\n\n", table);

						// TODO: evaluate *cmd (PUT/GET)
						// PUT: write data to DB
						write_in_db(mysql,DB_NAME,table,value);
						// GET: read from DB (and send value to node requesting data)

						free(table);
					}
					free(cmdSensorPos);
				}
				free(sensornameAndValue);
			}
			else
			{
				// received own message
			}
		}

	}	// while loop

	printf("SOCKET shutting down\n");

	shutdown(sock, 2);
	close(sock);

	return NULL;
}

/**
 * Main logic is implemented here.<br/>
 * Reading sensor data and limits from database.<br/>
 * Based on data and limits make decision
 * to open or close window.
 * @param args not used
 */
void *decisionLoop(void* args)
{
	MYSQL  *mysql = mysql_init(NULL);					// initializing handle

	bool read_success = false;

	// sensor data variables
	double airP_in, airP_out, airP_max;					// air pressure
	double airQ_in, airQ_out,airQ_max;					// air quality (CO2?)
	double autoMode_bo;									// auto mode (bool)
	double humid_in, humid_out, humid_min, humid_max;	// humidity
	double manClose_bo, manOpen_bo;						// manually open/close (bool)
	double temp_in, temp_out, temp_max, temp_min;		// temperature
	double vol_in, vol_out, vol_max;					// volume (street noise)
	double win_open_bo;									// windows status (bool)

	// Implementation Grenzwerte.png
	double airQ_temp, airQ_temp2;
	double temp_average;
	double humid_average;
	int priority;
	bool airQ_alarm;									// air quality alarm
	bool temp_alarm;									// temperature alarm
	bool humid_alarm;									// humidity alarm
	bool vol_alarm;										// volumen alarm
	bool wind_alarm;									// wind alarm (actually not imptemented)
	bool airQ_alarm_changed;
	bool temp_alarm_changed;
	bool humid_alarm_changed = false;
	bool humid_alarm_rising_edge = false;
	bool humid_alarm_falling_edge = false;
	bool vol_alarm_changed;
	
	// Implementation State Machine
	bool init = true;
	bool state0 = false;
	bool state11 = false;
	bool state12 = false;
	bool state13 = false;
	bool state14 = false;
	bool state21 = false;
	bool state22 = false;
	bool state23 = false;
	bool state24 = false;
	bool state31 = false;
	bool state32 = false;
	bool state33 = false;
	bool state34 = false;
	bool state41 = false;
	bool state42 = false;
	bool state43 = false;
	bool state44 = false;

	// start while(1)
	// periodically read data from DB
	while(1) {

		// Auto/Manual
		autoMode_bo = get_latest_value_bool(mysql,DB_NAME,TBL_AUTO, &read_success);
		manClose_bo = get_latest_value_bool(mysql,DB_NAME,TBL_MAN_CLOSE, &read_success);
		manOpen_bo = get_latest_value_bool(mysql,DB_NAME,TBL_MAN_OPEN, &read_success);
		// current status
		win_open_bo = get_latest_value_bool(mysql,DB_NAME,TBL_WIN_STATUS, &read_success);
		// write current controll status in database
		// Air Quality
		if (airQ_alarm)
		{
			// Air Quality Alarm
			writeAirQState("2");
		} else {
			// Air Qualitiy OK
			writeAirQState("0");
		}

		// Temperature
		if (temp_alarm)
		{
			// Temperature Alarm
			writeTemperatureState("2");
		} else {
			if ((temp_min < temp_in) && (temp_in < temp_max))
			{
				// Temparture OK
				writeTemperatureState("0");
			} else {
				// Temperature Warning
				writeTemperatureState("1");
			}
		}
		// Humidity Alarm
		if (humid_alarm && humid_alarm_changed)
		{
			// Humidity Alarm
			writeHumdityState("2");
			if ((humid_min < humid_in) && (humid_in < humid_max))
			{
				// Humidity OK
				writeHumdityState("0");
			} else {
				// Humidity Warning
				writeHumdityState("1");
			}
		}
		// Noise Alarm
		if (vol_alarm)
		{
			// Noise ALarm
			writeVolumeState("2");
		} else {
			// Noise OK
			writeVolumeState("0");
		}

		//
		// Sensor Data
		//
		// Air Pressure
		airP_in = get_latest_value_double(mysql,DB_NAME,CONCAT_TABLE(SENS_AIR_PRESSURE,POS_IN), &read_success);
		airP_out = get_latest_value_double(mysql,DB_NAME,CONCAT_TABLE(SENS_AIR_PRESSURE,POS_OUT), &read_success);
		airP_max = get_latest_value_int(mysql,DB_NAME,CONCAT_TABLE(SENS_AIR_PRESSURE,THR_MAX), &read_success);
		// Air Quality
		airQ_in = get_latest_value_double(mysql,DB_NAME,CONCAT_TABLE(SENS_AIR_QUALITY,POS_IN), &read_success);
		airQ_out = get_latest_value_double(mysql,DB_NAME,CONCAT_TABLE(SENS_AIR_QUALITY,POS_OUT), &read_success);
		airQ_max = get_latest_value_int(mysql,DB_NAME,CONCAT_TABLE(SENS_AIR_QUALITY,THR_MAX), &read_success);
		// Humidity
		humid_in = get_latest_value_double(mysql,DB_NAME,CONCAT_TABLE(SENS_HUMIDITY,POS_IN), &read_success);
		humid_out = get_latest_value_double(mysql,DB_NAME,CONCAT_TABLE(SENS_HUMIDITY,POS_OUT), &read_success);
		humid_min = get_latest_value_int(mysql,DB_NAME,CONCAT_TABLE(SENS_HUMIDITY,THR_MIN), &read_success);
		humid_max = get_latest_value_int(mysql,DB_NAME,CONCAT_TABLE(SENS_HUMIDITY,THR_MAX), &read_success);
		humid_average = (((humid_max - humid_min) / 2) + humid_min);
		// Temperature
		temp_in = get_latest_value_double(mysql,DB_NAME,CONCAT_TABLE(SENS_TEMP,POS_IN), &read_success);
		temp_out = get_latest_value_double(mysql,DB_NAME,CONCAT_TABLE(SENS_TEMP,POS_OUT), &read_success);
		temp_min = get_latest_value_int(mysql,DB_NAME,CONCAT_TABLE(SENS_TEMP,THR_MIN), &read_success);
		temp_max = get_latest_value_int(mysql,DB_NAME,CONCAT_TABLE(SENS_TEMP,THR_MAX), &read_success);
		temp_average = ((temp_max -temp_min) / 2) + temp_min;
		// Volume
		vol_in = get_latest_value_double(mysql,DB_NAME,CONCAT_TABLE(SENS_VOLUME,POS_IN), &read_success);
		vol_out = get_latest_value_double(mysql,DB_NAME,CONCAT_TABLE(SENS_VOLUME,POS_OUT), &read_success);
		vol_max = get_latest_value_int(mysql,DB_NAME,CONCAT_TABLE(SENS_VOLUME,THR_MAX), &read_success);

		//
		// Logical data for internal control
		//
		priority = get_latest_value_int(mysql,DB_NAME, PRIORITY, &read_success);
		
		// Implementation of Grenzwerte.png
		// Air Quality Alarm
		if (((airQ_in + airQ_in) / 2) > airQ_max)
		{
			airQ_alarm = true;
		}
		if ((((airQ_in + airQ_in) / 2) == (airQ_max - 2)))
		{
			airQ_alarm = false; 	
		}

		// Temperature Alarm
		if (((temp_in < temp_min) && (temp_in < temp_out)) || ((temp_in > temp_max) && (temp_in > temp_out)))
		{
			temp_alarm = true;
		}
		if (temp_in == temp_average)
		{
			temp_alarm = false;
		}

		// Humidity Alarm
		if (((humid_in < humid_min) && (humid_in < humid_out)) || ((humid_in > humid_max) && (humid_in > humid_out)))
		{
			humid_alarm = true;
		}
		if (humid_in == humid_average)
		{
			humid_alarm = false;
		}

		// Volume Alarm
		if (vol_out > vol_max)
		{
			vol_alarm = true;
		}
		// Attention: To-Do!
		if ((vol_max - 2) == vol_out)
		{
			vol_alarm = false;
		}

		// Wind Alarm actually not implemented!
		wind_alarm = false;

		// Transition 0
		// At the beginning the Window will always be closed
		if (init && !win_open_bo && !state0)
		{
			init = false;
			state0 = true;
		} 

		// Transition 1.1
		if (state0 && (priority==0) && !state11)
		{
			state0 = false;
			state11 = true;		
		}

		// Transition 1.2
		if (state11 && (!wind_alarm && ((!autoMode_bo && manOpen_bo) || (autoMode_bo && humid_alarm))) && !state12)
		{
			state11 = false;
			state12 = true;
		}

		// Transition 1.3
		if (state12 && win_open_bo && !state13)
		{
			state12 = false;
			state13 = true;
		}

		// Transition 1.4
		if (state13 && (!wind_alarm || (!autoMode_bo && manClose_bo) || (autoMode_bo && (vol_alarm || !humid_alarm))) && !state14)
		{
			state13 = false;
			state14 = true;
		}

		// Transition 1.5
		if (state14 && !win_open_bo && !state11)
		{
			state14 = false;
			state11 = true;
		}

		// Transition 1.6
		if (state13 && (priority != 0) && !init)
		{
			state13 = false;
			init = true;
		}

		// Transition 1,7
		if (state11 && (priority != 0) && !init)
		{
			state11 = false;
			init = true;
		}

		// Transition 2.1
		if (state0 && (priority==1) && !state21)
		{
			state0 = false;
			state21 = true;		
		}

		// Transition 2.2
		if (state21 && (!wind_alarm && ((!autoMode_bo && manOpen_bo) || (autoMode_bo && temp_alarm))) && !state22)
		{
			state21 = false;
			state22 = true;
		}

		// Transition 2.3
		if (state22 && win_open_bo && !state23)
		{
			state22 = false;
			state23 = true;
		}

		// Transition 2.4
		if (state23 && (!wind_alarm || (!autoMode_bo && manClose_bo) || (autoMode_bo && (vol_alarm || !temp_alarm))) && !state24)
		{
			state23 = false;
			state24 = true;
		}

		// Transition 2.5
		if (state24 && !win_open_bo && !state21)
		{
			state24 = false;
			state21 = true;
		}

		// Transition 2.6
		if (state23 && (priority != 1) && !init)
		{
			state23 = false;
			init = true;
		}

		// Transition 2,7
		if (state21 && (priority != 1) && !init)
		{
			state21 = false;
			init = true;
		}

		// Transition 3.1
		if (state0 && (priority==2) && !state31)
		{
			state0 = false;
			state31 = true;		
		}

		// Transition 3.2
		if (state31 && (!wind_alarm && ((!autoMode_bo && manOpen_bo) || (autoMode_bo && airQ_alarm))) && !state32)
		{
			state31 = false;
			state32 = true;
		}

		// Transition 3.3
		if (state32 && win_open_bo && !state33)
		{
			state32 = false;
			state33 = true;
		}

		// Transition 3.4
		if (state33 && (!wind_alarm || (!autoMode_bo && manClose_bo) || (autoMode_bo && (vol_alarm || !airQ_alarm))) && !state34)
		{
			state33 = false;
			state34 = true;
		}

		// Transition 3.5
		if (state34 && !win_open_bo && !state31)
		{
			state34 = false;
			state31 = true;
		}

		// Transition 3.6
		if (state33 && (priority != 2) && !init)
		{
			state33 = false;
			init = true;
		}

		// Transition 3,7
		if (state31 && (priority != 2) && !init)
		{
			state31 = false;
			init = true;
		}


		// Transition 4.1
		if (state0 && (priority==3) && !state41)
		{
			state0 = false;
			state41 = true;		
		}

		// Transition 4.2
		if (state41 && (!wind_alarm && ((!autoMode_bo && manOpen_bo) || (autoMode_bo && (airQ_alarm || temp_alarm || humid_alarm))) && !state42))
		{
			state41 = false;
			state42 = true;
		}

		// Transition 4.3
		if (state42 && win_open_bo && !state43)
		{
			state42 = false;
			state43 = true;
		}

		// Transition 4.4
		if (state43 && (state41 && (!wind_alarm && ((!autoMode_bo && manOpen_bo) || (autoMode_bo && (!airQ_alarm || !temp_alarm || !humid_alarm))) && !state44)))
		{
			state33 = false;
			state34 = true;
		}

		// Transition 4.5
		if (state44 && !win_open_bo && !state41)
		{
			state44 = false;
			state41 = true;
		}

		// Transition 4.6
		if (state43 && (priority != 3) && !init)
		{
			state43 = false;
			init = true;
		}

		// Transition 3,7
		if (state41 && (priority != 3) && !init)
		{
			state41 = false;
			init = true;
		}

		// Assignments
		if (init)
		{
			sw_send(DST_MULITCAST, PUT_WIN_CLOSE);
			sleep(5);
		}

		if(state12)
		{
			sw_send(DST_MULITCAST, "PUT_Win/Open");
			sleep(5);
		}

		if (state14)
		{
			sw_send(DST_MULITCAST, PUT_WIN_CLOSE);
			sleep(5);
		}

		if(state22)
		{
			sw_send(DST_MULITCAST, "PUT_Win/Open");
			sleep(5);
		}

		if (state24)
		{
			sw_send(DST_MULITCAST, PUT_WIN_CLOSE);
			sleep(5);
		}

		if(state32)
		{
			sw_send(DST_MULITCAST, "PUT_Win/Open");
			sleep(5);
		}

		if (state34)
		{
			sw_send(DST_MULITCAST, PUT_WIN_CLOSE);
			sleep(5);
		}

		if(state42)
		{
			sw_send(DST_MULITCAST, "PUT_Win/Open");
			sleep(5);
		}

		if (state44)
		{
			sw_send(DST_MULITCAST, PUT_WIN_CLOSE);
			sleep(5);
		}

		// Humidity Alarm changed 
		// (rising edge)
		if (humid_alarm && humid_alarm_rising_edge)
		{
			humid_alarm_changed = true;
			printf("Humidity Alarm changed \n");
		}
		humid_alarm_rising_edge = humid_alarm;
		// (falling edge)
		if (!humid_alarm && humid_alarm_falling_edge){
			humid_alarm_changed = true;
			printf("Humidity Alarm changed \n");
		}
		humid_alarm_falling_edge = humid_alarm;

	}

 return NULL;
}


/**
 * writes ip address of an interface to \p ip<br/>
 * adapted from (with GOTO!!):<br/>
 * http://valileo-valilei.blogspot.de/2010/09/getting-link-local-addres-from.html
 * @param if_name name of interface
 * @param if_name_length length of interface name
 * @param ip struct to save ip address to
 * @return 0 on success, negative on failure
 */
int get_link_local_addr(char* if_name, int if_name_length, struct sockaddr_in6* ip)
{
	struct ifaddrs *ifaddr, *ifa;
	int ret = -2;

	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		ret = -1;
		freeifaddrs(ifaddr);
		return ret;
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr->sa_family != AF_INET6) continue;
		if (strncmp(ifa->ifa_name, if_name, if_name_length)) continue;

		struct sockaddr_in6 *current_addr = (struct sockaddr_in6 *) ifa->ifa_addr;
		memcpy(ip, current_addr, sizeof(*current_addr));
		ret = 0;
		break;
	}
	freeifaddrs(ifaddr);
	return ret;
}

/**
 * writes ipv6 address of lowpan0 interface into \p ipv6_strBuf
 * @param ipv6_strBuf buffer to write ip address to
 * @param bufferLen length of buffer
 * @return 0 on success, 1 on failure
 */
int get_Lowpan0Ipv6Address(char* ipv6_strBuf, size_t bufferLen)
{
	//char srv_addr_str[IPV6_ADDR_MAX_STR_LEN];
	struct sockaddr_in6 srvLowpan0;

	// TODO: hardcoded interface
	if(get_link_local_addr("lowpan0",7,&srvLowpan0) != 0)
	{
		return 1;
	}
	if(inet_ntop(AF_INET6, &(srvLowpan0.sin6_addr), ipv6_strBuf, bufferLen) == NULL)
	{
		perror("inet_ntop");
		return 1;
	}

	return 0;
}

/**
 * thread sending data for testing purposes
 * when no RIOT node is available
 * @param args not used
 */
void *dummyLoop(void* args)
{
    time_t epoch_time;
    struct tm *tm_p;
    epoch_time = time( NULL );
    tm_p = localtime( &epoch_time );
    printf("The time is %.2d:%.2d:%.2d\n",
        tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec );

    char val[10];

	while(1)
	{
		epoch_time = time( NULL );
		tm_p = localtime( &epoch_time );

		sprintf(val,"%i",tm_p->tm_sec);

		//sw_send(DST_MULITCAST, PUT_CMD(SENS_TEMP,POS_IN,"0.1234"));
		sw_send(DST_MULITCAST, PUT_CMD(SENS_TEMP,POS_IN,val));
		sleep(3);
	}

	return NULL;
}

/**
 * splits a c-string into \p result<br/>
 * see:<br/>
 * http://stackoverflow.com/questions/11198604/c-split-string-into-an-array-of-strings
 * http://stackoverflow.com/questions/1398307/how-can-i-allocate-memory-and-return-it-via-a-pointer-parameter-to-the-calling
 * @param string string to split
 * @param delimiter delimiter to split string with
 * @param result array containing substrings
 * @return number of substrings
 */
int splitString(char* string, char* delimiter, char ***result)
{
	int num_subStr = 0;

	char *token = strtok(string, delimiter);
	while(token)
	{
		*result = (char**)realloc(*result, sizeof(char*) * (++num_subStr));

		if(*result==NULL)
		{
			return -1;//mem alloc failed
		}

		(*result)[num_subStr-1] = token;
		token = strtok(NULL, delimiter);
	}

	*result = (char**)realloc(*result, sizeof(char*) * (num_subStr+1));
	(*result)[num_subStr] = 0;

	return num_subStr;
}

/**
 * concatenates two c-strings<br/>
 * see:<br/>
 * http://stackoverflow.com/questions/8465006/how-to-concatenate-2-strings-in-c
 * @param s1 first string
 * @param s2 second string
 * @return concatenated string
 */
char* concat(char *s1, char *s2)
{
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char *result = (char*)malloc(len1+len2+1);//+1 for the zero-terminator
    if(result == NULL)
    {
    	return NULL;
    }
    //in real code you would check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result+len1, s2, len2+1);//+1 to copy the null-terminator
    return result;
}

/**
 * Concatenates two c-strings with a string separator
 * to create the appropriate table name for usage
 * with mysql database.
 * @param sensor name of sensor
 * @param suffix position of sensor
 * @return concatenated c-string
 */
char* tableName(char* sensor, char* suffix)
{
	char* sensor_ = concat(sensor, SEP_SENSOR);
	return concat(sensor_,suffix);
}

void writeAirQState(char* state)
{
	MYSQL  *mysql = mysql_init(NULL);					// initializing handle
	write_in_db(mysql,DB_NAME,CONCAT_TABLE(SENS_AIR_QUALITY,TBL_SUF_ALARMSTATE),state);
}

void writeHumdityState(char* state)
{
	MYSQL  *mysql = mysql_init(NULL);					// initializing handle
	write_in_db(mysql,DB_NAME,CONCAT_TABLE(SENS_HUMIDITY,TBL_SUF_ALARMSTATE),state);
}

void writeTemperatureState(char* state)
{
	MYSQL  *mysql = mysql_init(NULL);					// initializing handle
	write_in_db(mysql,DB_NAME,CONCAT_TABLE(SENS_TEMP,TBL_SUF_ALARMSTATE),state);
}

void writeVolumeState(char* state)
{
	MYSQL  *mysql = mysql_init(NULL);					// initializing handle
	write_in_db(mysql,DB_NAME,CONCAT_TABLE(SENS_VOLUME,TBL_SUF_ALARMSTATE),state);
}
