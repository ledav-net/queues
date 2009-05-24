
OBJS:=		queues_main.o queues.o

.SUFFIXES:      .c .o

queues:		$(OBJS) queues.h
		gcc -ggdb -o queues $(OBJS)

.c.o:
		gcc -ggdb -Wall -c $*.c
			
clean:
		@rm -vf queues $(OBJS) *~ 
