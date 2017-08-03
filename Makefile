CFLAGS  = -g -std=gnu99 -Wall -Werror 
CFLAGS += -I/opt/redpitaya/include
CFLAGS += -I/home/redpitaya/RedPitaya/BitBanging
LDFLAGS = -L/opt/redpitaya/lib
LDLIBS = -lm -lpthread -lrp -lgsl -lgslcblas -lpthread

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=)

all: $(OBJS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	$(RM) *.o
	$(RM) $(OBJS)
