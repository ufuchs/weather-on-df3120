#
# Copyright(c) 2013 Uli Fuchs <ufuchs@gmx.com>
# MIT Licensed
#

# this script is intended for building your software at development time 
# independent of the `minifs` build process at deployment time.

BASE := /embedded/minifs-df3120

include minifs-df3120.mk

### your DF3120 project starts here ###

TARGET=sdlweather
OBJECTS=datatypes.o picframe.o $(TARGET).o

all: clean $(TARGET) deploy

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(TARGET_CPPFLAGS)  $(LDFLAGS_RLINK) -lSDL -lSDL_image -lSDL_ttf -o $@ $(OBJECTS) 

%.o: %.c
	$(CC) $(CFLAGS) $(TARGET_CPPFLAGS) -c $< 

strip: $(TARGET)
	$(STRIP) --strip-all $(TARGET)

clean:
	rm -rf $(OBJECTS) $(TARGET)
	
deploy:
	cp sdlweather /exports/df3120-rootfs/usr/local/bin

