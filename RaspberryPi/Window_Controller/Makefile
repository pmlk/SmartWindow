#
#g++ main_controller.cpp schreiben_lesen.h SmartWindowMacros.h -o main -Wno-write-strings -lmysqlclient -pthread
#

CFLAGS = -Wno-write-strings
LIBS = -lmysqlclient
LDFLAGS = -pthread

OBJECTS = main_controller.cpp schreiben_lesen.h SmartWindowMacros.h
TARGET = main_controller

all: $(TARGET) 

$(TARGET) : $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(CFLAGS) $(LIBS) $(LDFLAGS)




