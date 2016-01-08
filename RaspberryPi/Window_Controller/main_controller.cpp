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
//#include "schreiben_lesen.h"
#include "write_read.h"
// when Cross-Compiling
#include "mysql/headers/mysql.h"
// when compiling on RasPi
//#include <mysql/mysql.h>


// from RIOT:
// http://riot-os.org/api/group__net__ipv6__addr.html#ga2713917d7f8462406ab96eb14f1bfa2c
#define IPV6_ADDR_MAX_STR_LEN   (sizeof("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"))
#define MAXBUF					(2048)

// MYSQL
#define DB_NAME					"SmartWindow"
#define THR_MIN					"MIN"
#define	THR_MAX					"MAX"

// Table Names
#define TBL_AUTO						"AutoModus"
#define TBL_MAN_OPEN					"ManOpen"
#define TBL_MAN_CLOSE					"ManClose"
#define TBL_WIN_STATUS					"Win_Open"
#define CONCAT_TABLE(sensor,suffix)		tableName(sensor,suffix)

//
// local functions
//
// thread functions (loops)
void *receiveLoop(void* args);
void *decisionLoop(void* args);
void *dummyLoop(void* args);

int sw_send(char *dst_addr, char *data);
int get_link_local_addr(char* if_name, int if_name_length, struct sockaddr_in6 *ip);
int get_Lowpan0Ipv6Address(char* ipv6_strBuf, size_t bufferLen);
int splitString(char* string, char* delimiter, char ***resultArr);
char* concat(char *s1, char *s2);
char* tableName(char* sensor, char* suffix);


/**
 * starts threads and
 * periodically sends multicast requests
 * for sensor data
 */
int main(int argc, char **argv)
{
	// declare and start threads
	pthread_t rcvThread, decisionThread;//, dummyDataThread;//readThread;
	pthread_create(&rcvThread, NULL, &receiveLoop, (void*)NULL);		//
	pthread_create(&decisionThread, NULL, &decisionLoop, (void*)NULL);
	//pthread_create(&readThread, NULL, &readLoop, (void*)NULL);
	//pthread_create(&dummyDataThread, NULL, &dummyLoop, (void*)NULL);

	sleep(1);
	// detect nodes
	//sw_send("ff02::1", SW_PING);

	// requesting sensor data periodically
	while(1)
	{
		// request data
		sw_send(DST_MULITCAST, GET_ALL);
		sleep(5);
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

	// clear buffer memory
	memset(buffer,0, MAXBUF);

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

			//if(1)
			// ignore messages from self, comment out (use if(1)) when using dummy loop!
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

		// clear buffer
		for(int i = 0; i < status; i++)
		{
			buffer[i] = 0;
		}
	}	// while loop

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
/**/
	MYSQL  *mysql = mysql_init(NULL);					// initializing handle
	//struct CGI_DATEN *messdaten = new CGI_DATEN();

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
	bool airQ_alarm;									// air quality alarm
	bool temp_alarm;									// temperature alarm
	bool humid_alarm;									// humidity alarm
	bool vol_alarm;

	// Implementation State Machine
	bool init = true;
	bool state1 = false;
	bool state2 = false;
	bool state3 = false;
	bool state4 = false;


	// start while(1)
	// periodically read data from DB
	while(1) {

		// Auto/Manual
		autoMode_bo = get_latest_value_bool(mysql,DB_NAME,TBL_AUTO, &read_success);
		manClose_bo = get_latest_value_bool(mysql,DB_NAME,TBL_MAN_CLOSE, &read_success);
		manOpen_bo = get_latest_value_bool(mysql,DB_NAME,TBL_MAN_OPEN, &read_success);
		// current status
		win_open_bo = get_latest_value_bool(mysql,DB_NAME,TBL_WIN_STATUS, &read_success);



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
//		// Implementation of Grenzwerte.png
//		// Air Quality Alarm
//		if (((airQ_out + airQ_in) / 2) > airQ_max)
//		{
//			airQ_alarm = true;
//		}
//		if (((airQ_out + airQ_in) / 2) == (airQ_max - 2))
//		{
//			airQ_alarm = false;
//		}
//
//		// Temperature Alarm
//		if (((temp_in < temp_min) && (temp_in < temp_out)) || ((temp_in > temp_max) && (temp_in > temp_out)))
//		{
//			temp_alarm = true;
//		}
//		if (temp_in == temp_average)
//		{
//			temp_alarm = false;
//		}
//
//		// Humidity Alarm
//		if (((humid_in < humid_min) && (humid_in < humid_out)) || ((humid_in > humid_max) && (humid_in > humid_out)))
//		{
//			humid_alarm = true;
//		}
//		if (humid_in == humid_average)
//		{
//			humid_alarm = false;
//		}
//
//		// Volume Alarm
//		if ((vol_in > vol_max) && (vol_in > vol_out))
//		{
//			vol_alarm = true;
//		}
//		if (vol_in < vol_max)
//		{
//			vol_alarm = false;
//		}
//
//		// Initialize
//		// At the beginning the Window will always be closed
//		if (init && !win_open_bo && !state1)
//		{
//			init = false;
//		} else {
//			state1 = true;
//		}
//
//		// State 1
//		// Windows is closed and waiting on Commands
//		if (state1 && !state2 && ((!autoMode_bo && manOpen_bo) || (manOpen_bo && (airQ_alarm && temp_alarm && humid_alarm))))
//		{
//			state1 = false;
//		} else {
//			state2 = true;
//		}
//
//		// State 2
//		// Open Window till it is open
//		if (state2 && !state3 && win_open_bo)
//		{
//			state2 = false;
//		} else {
//			state3 = true;
//		}
//
//		// State 3
//		// Window is opened and waiting on commands
//		if (state3 && !state4 && ((!autoMode_bo && manClose_bo) || (autoMode_bo && (vol_alarm || (!humid_alarm && airQ_alarm && temp_alarm)))))
//		{
//			state3 = false;
//		} else {
//			state4 = true;
//		}
//
//		// State 4
//		// Close Window till it is closed
//		if (state4 && !state1 && !win_open_bo)
//		{
//			state4 = false;
//		} else {
//			state1 = true;
//		}
//
//		// Assignments
//		if (init)
//		{
//			sw_send(DST_MULITCAST, "PUT_Win/Close");
//			sleep(5);
//		}
//
//		if(state2)
//		{
//			sw_send(DST_MULITCAST, "PUT_Win/Open");
//			sleep(5);
//		}
//
//		if (state4)
//		{
//			sw_send(DST_MULITCAST, "PUT_Win/Close");
//			sleep(5);
//		}
//
//	}

/*
	while(1)
	{
		sw_send(DST_MULITCAST, "PUT_Win/Open");
		sleep(5);

		sw_send(DST_MULITCAST, "PUT_Win/Close");
		sleep(5);
	}
*/

 return NULL;
}


/**
 * writes ip address of an interface to \p ip<br/>
 * see (with GOTO!!):<br/>
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
		//WTF:
		//goto end;
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
		//ifa = NULL;
		break;

		//WTF:
		//goto end;
	}
	//WTF:
	//end:
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
