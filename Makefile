TARGET=server
CC=gcc
CFLAGS+= -ansi -Wall -Wextra -Wdeclaration-after-statement -Wbad-function-cast -Wcast-qual -Wstrict-prototypes -Wmissing-declarations -Wmissing-prototypes -Wno-unused-parameter -Werror -pedantic -g
SOURCES=$(shell echo *.c)
OBJECTS=$(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ 

clean:
	rm -f $(TARGET) $(OBJECTS) .depend *~

.depend: *.[ch]
	$(CC) -MM *.c >.depend

-include .depend

.PHONY: clean all

