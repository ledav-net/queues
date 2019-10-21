
OBJS:=		queues_unit-test.o queues.o
CFLAGS:=

.SUFFIXES:      .c .o

queues:		$(OBJS) queues.h
		gcc $(CFLAGS) -ggdb -o queues $(OBJS)

.c.o:
		gcc $(CFLAGS) -ggdb -Wall -c $*.c
			
clean:
		@rm -vf queues $(OBJS) *~ 
