TARGET=sdlweather
OBJECTS=datatypes.o picframe.o gpio.o window.o $(TARGET).o

CFLAGS  := ${CFLAGS} -std=c99 -I../staging/usr/include

LDFLAGS := ${LDFLAGS_RLINK} -lSDL -lSDL_image -lSDL_ttf

CC      = ccfix arm-v4t-linux-uclibcgnueabi-gcc

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) 

%.o: %.c
	$(CC) $(CFLAGS) -c $< 

strip: $(TARGET)
	strip --strip-all $(TARGET)

install:


clean:
	rm -rf $(OBJECTS) $(TARGET)
