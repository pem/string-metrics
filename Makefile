#
# pem 2018-07-07
#

CC=gcc --std=c99

CCOPTS=-pedantic -Wall -Werror

CCDEFS=-D_POSIX_C_SOURCE=2

#CFLAGS=-g $(CCOPTS) $(CCDEFS) #-DDEBUG
CFLAGS=-O $(CCOPTS) $(CCDEFS)

PROG1=smtest
PROG2=edist

LIB=libsm.a

LSRC=edit-distance.c lcsubstr.c de1337.c
PSRC=smtest.c edist.c

LOBJ=$(LSRC:%.c=%.o)
OBJ=$(LOBJ) $(PSRC:%c=%o)

all:	$(PROG1) $(PROG2)

$(PROG1):	$(PROG1).o $(LIB)

$(PROG2):	$(PROG2).o $(LIB)

$(LIB):	$(LOBJ)
	rm -f $(LIB)
	$(AR) qc $(LIB) $(LOBJ)
	ranlib $(LIB)

clean:
	$(RM) $(OBJ) core

cleanall:	clean
	$(RM) $(PROG1) $(PROG2) $(LIB) make.deps

make.deps:
	gcc -MM $(CFLAGS) $(LSRC) $(PSRC) > make.deps

include make.deps
