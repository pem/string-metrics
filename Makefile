#
# pem 2018-07-07, 2023-05-07
#

CC=gcc --std=c99

CCOPTS=-pedantic -Wall -Werror

CCDEFS=-D_POSIX_C_SOURCE=2

#CFLAGS=-g $(CCOPTS) $(CCDEFS) #-DDEBUG
CFLAGS=-O $(CCOPTS) $(CCDEFS)

PROG1=smtest
PROG2=smtest-wchar

LIB=libsm.a

LSRC=edit-distance.c lcsubstr.c
PSRC=smtest.c edist.c

LOBJ=$(LSRC:%.c=%.o) $(LSRC:%.c=%-wchar.o)
OBJ=$(LOBJ) $(PSRC:%.c=%.o) $(PSRC:%.c=%-wchar.o)

%-wchar.o: %.c
	$(CC) $(CFLAGS) -DSM_WCHAR -c -o $@ $<

all:	$(PROG1) $(PROG2) $(PROG3) $(PROG4)

$(PROG1):	$(PROG1).o $(LIB) sm-utf8.o

$(PROG2):	$(PROG2).o $(LIB) sm-utf8.o

$(LIB):	$(LOBJ)
	rm -f $(LIB)
	$(AR) qc $(LIB) $(LOBJ)
	ranlib $(LIB)

edit-distance.o: edit-distance.c string-metrics.h
lcsubstr.o: lcsubstr.c string-metrics.h
smtest.o: smtest.c string-metrics.h sm-utf8.h

edit-distance-wchar.o: edit-distance.c string-metrics.h
lcsubstr-wchar.o: lcsubstr.c string-metrics.h
smtest-wchar.o: smtest.c string-metrics.h sm-utf8.h
sm-utf8.o: sm-utf8.c sm-utf8.h

clean:
	$(RM) $(OBJ) sm-utf8.o core

cleanall:	clean
	$(RM) $(PROG1) $(PROG2) $(LIB)
