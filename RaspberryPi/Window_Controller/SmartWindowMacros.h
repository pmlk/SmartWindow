/*
 * SmartWindowMacros.h
 *
 *  Created on: Dec 4, 2015
 *      Author: Patrick
 */

#ifndef SMARTWINDOWMACROS_H_
#define SMARTWINDOWMACROS_H_


// Networking
#define DST_MULITCAST	"ff02::1"
#define PORT			6414

// PING PONG Messages
#define SW_PING		"SW_PING"
#define SW_PONG		"SW_PONG"

// Sensor Names
#define SENS_ALL			"All"
#define SENS_TEMP			"Temp"
#define SENS_AIR_PRESSURE	"AirPressure"
#define SENS_AIR_QUALITY	"AirQuality"
#define SENS_VOLUME			"Volume"
#define SENS_HUMIDITY		"Humidity"
#define PRIORITY			"Priority"

// Actuator Names
#define ACT_WINDOW			"Win"

// commands
#define CMD_GET				"GET"
#define CMD_PUT				"PUT"

// window values
#define OPEN				"Open"
#define CLOSE				"Close"

// position of sensor node (inside, outside)
#define POS_IN				"IN"
#define POS_OUT				"OUT"
// string separators
#define SEP_SENSOR			"_"
#define SEP_VALUE			"/"

// Commands
#define GET_CMD(sensor,position)		CMD_GET SEP_SENSOR sensor SEP_SENSOR position
#define PUT_CMD(sensor,position,value)	concat(concat(concat(CMD_PUT, SEP_SENSOR),tableName(sensor,position)), concat(SEP_VALUE, value))
#define PUT_WIN_OPEN					CMD_PUT SEP_SENSOR ACT_WINDOW SEP_VALUE OPEN
#define PUT_WIN_CLOSE					CMD_PUT SEP_SENSOR ACT_WINDOW SEP_VALUE CLOSE
#define GET_ALL							CMD_GET SEP_SENSOR SENS_ALL



#endif /* SMARTWINDOWMACROS_H_ */
