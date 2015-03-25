CC = gcc
XX = g++
CFLAGS = -Wall -g -O3  -DNDEBUG
INCLUDES += -I/usr/include
LIBS = -L/usr/local/lib

TARGET = web_server_cgi

%.o: %.c

	$(CC)  $(CFLAGS) $(INCLUDES) -c $< -o $@ 

%.o:%.cpp

	$(XX)  $(CFLAGS) $(INCLUDES) -c $< -o $@ 


SOURCES = $(wildcard *.c *.cpp)
OBJS = $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))

$(TARGET) : $(OBJS)
	$(XX) $(OBJS) $(LIBS)  -o $(TARGET) -lpthread

clean:

	rm -rf *.o web_server_cgi
