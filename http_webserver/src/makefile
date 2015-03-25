CC = gcc
XX = g++
CFLAGS = -Wall -g -O3  -DNDEBUG
INCLUDES += -I/usr/include
LIBS = -L/usr/local/lib

TARGET = http_webserver

%.o:%.cpp

	$(XX)  $(CFLAGS) $(INCLUDES) -c $< -o $@ 

SOURCES = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(SOURCES))

$(TARGET) : $(OBJS)
	$(XX) $(OBJS) $(LIBS)  -o $(TARGET) -lpthread

clean:

	rm -rf *.o http_webserver
