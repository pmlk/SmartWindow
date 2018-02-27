# SmartWindow
An automated window controller

## Our Mission
This project is part of a course at the University of Applied Sciences (HAW) Hamburg. The general purpose of the course is to find ways to enhance the classroom environment using [RIOT](http://www.riot-os.org). The easiest way to counteract any bad classroom atmosphere is to open or close one or multiple windows. Thus increasing the students' ability to concentrate.

Multiple factors can be taken into consideration to determine whether a window should be opened or closed:

* **air quality**
* **temperature**
* **street noise**
* **humidity**
* rain
* outside windspeed

To accomplish our goal we defined criteria that have to be met to trigger the opening or closing of a window. Sensors measure the physical value of the aforementioned factors. And actuators physically open and close a window. 

For the purpose of this course we used a simple window model consisting of an aluminum frame and a picture sized glass plate.

## Hardware
The following hardware is used:

* RaspberrPi B+ with 6LoWPAN radio:
	* [OpenLabs Raspberry Pi 802.15.4 radio](http://openlabs.co/store/Raspberry-Pi-802.15.4-radio)
* [Phytec phyWAVE KW22] (https://github.com/RIOT-OS/RIOT/wiki/Board%3A-Phytec-phyWAVE-KW22) with the following sensors:
	*  MPL3115A2 (temperature)
	*  SEN0114 (humidity)
	*  LMV324 (sound/noise)
	*  MQ_135 (gas sensor/air quality)
* SG90 (stepping motor)
* window model

## Software Architecture
The software in this project follows a simple layer architecure.
This is not strictly following the OSI model.

Layer | Software | Hardware
------|----------|---------
Presentation|HTML|RasPi
Application|Python Webserver|RasPi
Data Storage|MySQL Database|RasPi
Logic|Window Controller (C++)|RasPi
Network|C/C++ programs|RasPi/Phytec
Physical|C program|Phytec/Sensors

<img src="https://raw.githubusercontent.com/smartuni/SmartWindow/master/Doc/sw_architecture.png" alt="architecture" height="500">

[//]: #(![architecture](https://raw.githubusercontent.com/smartuni/SmartWindow/master/Doc/sw_architecture.png))
