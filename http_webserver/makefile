CC = gcc
XX = g++
CFLAGS = -Wall -g -O3  -DNDEBUG
INCLUDES += -I/usr/include
LIBS = -L/usr/local/lib

TARGET = bin/http_webserver

%.o:%.cpp

	$(XX)  $(CFLAGS) $(INCLUDES) -c $< -o $@ 

SOURCES = $(wildcard src/*.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SOURCES))

$(TARGET) : $(OBJS)
	$(XX) $(OBJS) $(LIBS)  -o $(TARGET) -lpthread

clean:

	rm -rf src/*.o 
	rm -rf bin/http_webserver
