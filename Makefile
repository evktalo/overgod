CC?=gcc
CFLAGS?=-O2 -march=native
CFLAGS_SURPRESS_WARNING?=-Wno-format-security
LDFLAGS?=-lm -lalleg

SOURCE=$(wildcard *.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCE))
DEPENDENCY=$(wildcard *.h) Makefile
TARGET=overgod

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c $(DEPENDENCY)
	$(CC) $< $(CFLAGS) $(CFLAGS_SURPRESS_WARNING) -c -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
