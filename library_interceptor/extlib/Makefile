CC	= gcc
CFLAGS	= -ggdb -Wall -D_GNU_SOURCE -std=c99

LIB	= libextlib.so.1
LIB_FLAGS = -fPIC -shared -lstdc++ -lsqlite3

RM	= rm
ECHO	= echo

SRCS	= main.c syscall_handler.c
OBJS	= ${SRCS:.c=.o} ${COM_SRCS:.c=.o} file_manager.o

.SUFFIXES:
.SUFFIXES: .o .c

all: $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(LIB_FLAGS) $(OBJS) -o $(LIB)

.c.o :
	$(OLIB) $(CC) $(CFLAGS) -c $<

file_manager.o:
	g++ -c ../file_manager/file_manager.cpp -o file_manager.o 

clean :
	-$(RM) -f $(OBJS)
	-$(RM) -f $(LIB)

