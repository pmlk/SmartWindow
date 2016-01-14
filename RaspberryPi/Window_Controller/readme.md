Compile the code:			

code for your raspberry pi:
- before you compile the code, you need to download the mysql headers and libs:  
	$ sudo apt-get install libmysqlclient-dev  


write_read.cpp - contains functions to write into the MySQL database and read from it

main_controller - manages the whole process: requests data of all sensors, writes it into the database, decides when to close and open the window and sends commands to the microcontoller

(edited by Simon Stallbaum)
