CC = gcc
RM = rm -f
ST = `which strip`

NAME = arpmonitor

INCLUDE = -I.
LIBS = 
CFLAGS = -Wall $(INCLUDE)

# files
OBJECTS = functions.o  \
		  main.o

SOURCES =  functions.c  \
		  main.c

# rules
.SUFFIXES: .c
.c.o:
		$(CC) $(CFLAGS) -c -o $@ $<

# build
all:	$(OBJECTS)
		$(CC) -o $(NAME) $(OBJECTS) $(LIBS)
		$(ST) $(NAME)

clean:
		$(RM) $(NAME) $(OBJECTS)
		$(RM) *~
